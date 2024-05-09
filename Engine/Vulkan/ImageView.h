#pragma once

#include "Engine/Vulkan/Common.h"

namespace Engine::Vulkan {
    class ImageView : public NonCopyMoveable {
    public:
        ImageView(Ref<IImage> image);
        ~ImageView();

        VkImageView Handle();
        VkExtent2D  GetExtent() const;
        Ref<IImage> GetImage() const;

    private:
        Ref<IImage> m_Image;
        VkImageView m_View;
    };

}  // namespace Engine::Vulkan