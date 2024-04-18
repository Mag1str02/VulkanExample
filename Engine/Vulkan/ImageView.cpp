#include "ImageView.h"

#include "Device.h"
#include "Image.h"

namespace Engine::Vulkan {

    ImageView::ImageView(Ref<IImage> image) : m_Image(image) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image                           = m_Image->Handle();
        viewInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format                          = m_Image->GetFormat();
        viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel   = 0;
        viewInfo.subresourceRange.levelCount     = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount     = 1;

        VK_CHECK(vkCreateImageView(m_Image->GetDevice()->GetLogicDevice(), &viewInfo, nullptr, &m_View));
    }
    ImageView::~ImageView() {
        vkDestroyImageView(m_Image->GetDevice()->GetLogicDevice(), m_View, nullptr);
    }

    VkImageView ImageView::Handle() {
        return m_View;
    }

}  // namespace Engine::Vulkan