#pragma once

#include "Common.h"

namespace Engine::Vulkan {
    class ImageView {
    public:
        ImageView(Ref<IImage> image);
        ~ImageView();

        VkImageView Handle();

    private:
        Ref<IImage> m_Image;
        VkImageView m_View;
    };

}  // namespace Engine::Vulkan