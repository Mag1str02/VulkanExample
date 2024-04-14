#pragma once

#include "Common.h"

namespace Vulkan {
    class ImageView {
    public:
        ImageView(Ref<Image> image);
        ~ImageView();

        VkImageView Handle();

    private:
        Ref<Image>  m_Image;
        VkImageView m_View;
    };

}  // namespace Vulkan