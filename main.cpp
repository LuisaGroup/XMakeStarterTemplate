//
// Created by Mike on 2024/5/21.
//

#include <luisa/luisa-compute.h>

// For the DSL sugar macros like $if.
// We exclude this header from <luisa-compute.h> to avoid pollution.
// So you have to include it explicitly to use the sugar macros.
#include <luisa/dsl/sugar.h>

// for std::cerr
#include <iostream>

// stb for image saving
#include <stb/stb_image_write.h>

using namespace luisa;
using namespace luisa::compute;

int main(int argc, char *argv[]) {

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <backend>\n";
        return 1;
    }

    // Step 1.1: Create a context
    Context context{argv[0]};

    // Step 1.2: Load the CUDA backend plug-in and create a device
    Device device = context.create_device(argv[1]);

    // Step 2.1: Create a stream for command submission
    Stream stream = device.create_stream();

    // Step 2.2: Create an 1024x1024 image with 4-channel 8-bit storage for each
    // pixel; the template
    //           argument `float` indicates that pixel values reading from or
    //           writing to the image are converted from `byte4` to `float4` or
    //           `float4` to `byte4` automatically
    Image<float> device_image =
        device.create_image<float>(PixelStorage::BYTE4, 1024u, 1024u, 0u);

    // Step 3.1: Define kernels to describe the device-side computation
    //
    //           A `Callable` is a function *entity* (not directly inlined during
    //           the AST recording) that is invocable from kernels or other
    //           callables
    Callable linear_to_srgb =
        [](Float4 /* alias for Var<float4> */ linear) noexcept {
            // The DSL syntax is much like the original C++
            auto x = linear.xyz();
            return make_float4(select(1.055f * pow(x, 1.0f / 2.4f) - 0.055f,
                                      12.92f * x, x <= 0.00031308f),
                               linear.w);
        };
    //           A `Kernel` is an *entry* function to the device workload
    Kernel2D fill_image_kernel =
        [&linear_to_srgb](
            ImageFloat /* alias for Var<Image<float>> */ image) noexcept {
            Var coord = dispatch_id().xy();
            Var rg = make_float2(coord) / make_float2(dispatch_size().xy());
            image->write(coord, linear_to_srgb(make_float4(rg, 1.0f, 1.0f)));
        };

    // Step 3.2: Compile the kernel into a shader (i.e., a runnable object on the
    // device)
    auto fill_image = device.compile(fill_image_kernel);

    // Prepare the host memory for holding the image
    std::vector<std::byte> download_image(1024u * 1024u * 4u);

    // Step 4: Generate commands from resources and shaders, and
    //         submit them to the stream to execute on the device
    stream << fill_image(device_image.view(0)).dispatch(1024u, 1024u)
           << device_image.copy_to(download_image.data())
           << synchronize();// Step 5: Synchronize the stream
    // Now, you have the device-computed pixels in the host memory!

    // Step 6: Save the image to a file
    stbi_write_png("color.png", 1024, 1024, 4, download_image.data(), 0);
}
