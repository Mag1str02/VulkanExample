#pragma once

#include "Object.h"

namespace Engine::Vulkan {
    class ImageView {
    public:
        ImageView(Ref<IImage> image);
        ~ImageView();

        VkImageView Handle();
        VkExtent2D  GetExtent() const;

    private:
        Ref<IImage> m_Image;
        VkImageView m_View;
    };

}  // namespace Engine::Vulkan