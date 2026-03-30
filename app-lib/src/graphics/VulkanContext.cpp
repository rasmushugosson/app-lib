#include "general/pch.h"

#ifdef AE_VULKAN

#include "VulkanContext.h"

#include <algorithm>

#include "DearImGui.h"

#include "backends/imgui_impl_vulkan.h"

ae::VulkanContext::VulkanContext(Window &window)
    : Context(window), m_FramesInFlight(0), m_Surface(VK_NULL_HANDLE), m_SwapChain(VK_NULL_HANDLE),
      m_SwapChainImageFormat(VK_FORMAT_UNDEFINED), m_SwapChainExtent(),
      m_ImGuiStandaloneRenderPass(VK_NULL_HANDLE), m_ImGuiOverlayRenderPass(VK_NULL_HANDLE),
      m_CommandPool(VK_NULL_HANDLE), m_CurrentFrame(0), m_CurrentImageIndex(0), m_NeedsResize(false)
{
}

ae::VulkanContext::~VulkanContext() = default;

ae::FrameInfo ae::VulkanContext::BeginFrame()
{
    if (m_NeedsResize)
    {
        RecreateSwapChain();
        m_NeedsResize = false;
    }

    VkDevice device = VulkanManager::Get().GetDevice();

    vkWaitForFences(device, 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);

    VkResult result = vkAcquireNextImageKHR(device, m_SwapChain, UINT64_MAX,
                                            m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE,
                                            &m_CurrentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapChain();
        // Retry acquire after recreation
        result = vkAcquireNextImageKHR(device, m_SwapChain, UINT64_MAX,
                                       m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE,
                                       &m_CurrentImageIndex);
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        AE_THROW_RUNTIME_ERROR("Failed to acquire Vulkan swapchain image");
    }

    vkResetFences(device, 1, &m_InFlightFences[m_CurrentFrame]);

    return { m_CurrentImageIndex, m_CurrentFrame, m_SwapChainExtent.width, m_SwapChainExtent.height };
}

void ae::VulkanContext::EndFrame(const VkCommandBuffer *appCommandBuffers, uint32_t appCBCount, bool hasImGui)
{
    std::vector<VkCommandBuffer> allCBs;
    allCBs.reserve(appCBCount + 1);

    if (appCBCount > 0)
    {
        allCBs.insert(allCBs.end(), appCommandBuffers, appCommandBuffers + appCBCount);
    }

    if (hasImGui)
    {
        if (appCBCount > 0)
        {
            allCBs.push_back(RecordImGuiOverlay());
        }
        else
        {
            allCBs.push_back(RecordImGuiStandalone());
        }
    }
    else if (appCBCount > 0)
    {
        allCBs.push_back(RecordTransitionToPresent());
    }

    if (!allCBs.empty())
    {
        VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[m_CurrentFrame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphores[m_CurrentFrame] };

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = static_cast<uint32_t>(allCBs.size());
        submitInfo.pCommandBuffers = allCBs.data();
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(VulkanManager::Get().GetGraphicsQueue(), 1, &submitInfo,
                          m_InFlightFences[m_CurrentFrame]) != VK_SUCCESS)
        {
            AE_THROW_RUNTIME_ERROR("Failed to submit Vulkan command buffers");
        }
    }

    // Present
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &m_RenderFinishedSemaphores[m_CurrentFrame];
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &m_SwapChain;
    presentInfo.pImageIndices = &m_CurrentImageIndex;

    VkResult result = vkQueuePresentKHR(VulkanManager::Get().GetGraphicsQueue(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        m_NeedsResize = true;
    }
    else if (result != VK_SUCCESS)
    {
        AE_THROW_RUNTIME_ERROR("Failed to present Vulkan image");
    }

    m_CurrentFrame = (m_CurrentFrame + 1) % m_FramesInFlight;
}

VkCommandBuffer ae::VulkanContext::RecordImGuiOverlay()
{
    VkCommandBuffer cmd = m_ImGuiCommandBuffers[m_CurrentImageIndex];
    vkResetCommandBuffer(cmd, 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    VK_CHECK(vkBeginCommandBuffer(cmd, &beginInfo));

    VkRenderPassBeginInfo rpInfo{};
    rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpInfo.renderPass = m_ImGuiOverlayRenderPass;
    rpInfo.framebuffer = m_ImGuiOverlayFramebuffers[m_CurrentImageIndex];
    rpInfo.renderArea.offset = { 0, 0 };
    rpInfo.renderArea.extent = m_SwapChainExtent;
    rpInfo.clearValueCount = 0;
    rpInfo.pClearValues = nullptr;

    vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

    vkCmdEndRenderPass(cmd);
    VK_CHECK(vkEndCommandBuffer(cmd));

    return cmd;
}

VkCommandBuffer ae::VulkanContext::RecordImGuiStandalone()
{
    VkCommandBuffer cmd = m_ImGuiCommandBuffers[m_CurrentImageIndex];
    vkResetCommandBuffer(cmd, 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    VK_CHECK(vkBeginCommandBuffer(cmd, &beginInfo));

    const auto &clearColorData = m_Window.GetClearColor();
    VkClearValue clearColor = { { { clearColorData[0], clearColorData[1], clearColorData[2], clearColorData[3] } } };

    VkRenderPassBeginInfo rpInfo{};
    rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpInfo.renderPass = m_ImGuiStandaloneRenderPass;
    rpInfo.framebuffer = m_ImGuiStandaloneFramebuffers[m_CurrentImageIndex];
    rpInfo.renderArea.offset = { 0, 0 };
    rpInfo.renderArea.extent = m_SwapChainExtent;
    rpInfo.clearValueCount = 1;
    rpInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

    vkCmdEndRenderPass(cmd);
    VK_CHECK(vkEndCommandBuffer(cmd));

    return cmd;
}

VkCommandBuffer ae::VulkanContext::RecordTransitionToPresent()
{
    VkCommandBuffer cmd = m_TransitionCommandBuffers[m_CurrentImageIndex];
    vkResetCommandBuffer(cmd, 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    VK_CHECK(vkBeginCommandBuffer(cmd, &beginInfo));

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = m_SwapChainImages[m_CurrentImageIndex];
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    barrier.dstAccessMask = 0;

    vkCmdPipelineBarrier(cmd,
                         VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                         VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                         0, 0, nullptr, 0, nullptr, 1, &barrier);

    VK_CHECK(vkEndCommandBuffer(cmd));

    return cmd;
}

bool ae::VulkanContext::CreateImpl()
{
    m_FramesInFlight = m_Window.GetDesc().framesInFlight;

    VulkanManager::Get().AddContext(m_Window.GetDesc().title);

    CreateSurface();

    m_GraphicsAPI = "Vulkan";
    m_GraphicsVersion = VulkanManager::Get().GetVersion();
    m_GraphicsCard = VulkanManager::Get().GetRenderer();
    m_GraphicsVendor = VulkanManager::Get().GetVendor();

    CreateSwapChain();
    CreateSwapChainImageViews();
    CreateImGuiStandaloneRenderPass();
    CreateImGuiOverlayRenderPass();
    CreateImGuiStandaloneFramebuffers();
    CreateImGuiOverlayFramebuffers();
    CreateCommandPool();
    CreateCommandBuffers();
    CreateSyncObjects();

    return true;
}

void ae::VulkanContext::ActivateImpl()
{
    // Does not need to be activated
}

void ae::VulkanContext::DeactivateImpl()
{
    // Does not need to be deactivated
}

void ae::VulkanContext::DestroyImpl()
{
    // Wait for all GPU work to complete
    vkDeviceWaitIdle(VulkanManager::Get().GetDevice());

    // Wait for all in-flight fences to ensure no pending operations
    if (!m_InFlightFences.empty())
    {
        vkWaitForFences(VulkanManager::Get().GetDevice(), static_cast<uint32_t>(m_InFlightFences.size()),
                        m_InFlightFences.data(), VK_TRUE, UINT64_MAX);
    }

    DestroySyncObjects();
    DestroyCommandBuffers();
    DestroyCommandPool();
    DestroyImGuiOverlayFramebuffers();
    DestroyImGuiStandaloneFramebuffers();
    DestroyImGuiOverlayRenderPass();
    DestroyImGuiStandaloneRenderPass();
    DestroySwapChainImageViews();
    DestroySwapChain();

    DestroySurface();

    VulkanManager::Get().RemoveContext();
}

void ae::VulkanContext::CreateSurface()
{
    VkInstance instance = VulkanManager::Get().GetInstance();

    if (glfwCreateWindowSurface(instance, m_Window.GetWindow(), nullptr, &m_Surface) != VK_SUCCESS)
    {
        AE_THROW_RUNTIME_ERROR("Failed to create Vulkan window surface");
    }

    ae::VulkanManager::Get().AddSurface(m_Surface);
}

void ae::VulkanContext::DestroySurface()
{
    ae::VulkanManager::Get().RemoveSurface(m_Surface);

    vkDestroySurfaceKHR(VulkanManager::Get().GetInstance(), m_Surface, nullptr);

    m_Surface = VK_NULL_HANDLE;
}

#undef max

void ae::VulkanContext::CreateSwapChain()
{
    VkSurfaceCapabilitiesKHR surfaceCapabilities;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(ae::VulkanManager::Get().GetPhysicalDevice(), m_Surface,
                                              &surfaceCapabilities);

    uint32_t imageCount = surfaceCapabilities.minImageCount + 1;

    if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
    {
        imageCount = surfaceCapabilities.maxImageCount;
    }

    uint32_t formatCount = 0;

    vkGetPhysicalDeviceSurfaceFormatsKHR(ae::VulkanManager::Get().GetPhysicalDevice(), m_Surface, &formatCount,
                                         nullptr);

    std::vector<VkSurfaceFormatKHR> formats(formatCount);

    vkGetPhysicalDeviceSurfaceFormatsKHR(ae::VulkanManager::Get().GetPhysicalDevice(), m_Surface, &formatCount,
                                         formats.data());

    VkSurfaceFormatKHR chosenFormat = formats[0];

    for (const auto &availableFormat : formats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
            availableFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
        {
            chosenFormat = availableFormat;
            break;
        }
    }

    m_SwapChainImageFormat = chosenFormat.format;

    uint32_t presentModeCount = 0;

    vkGetPhysicalDeviceSurfacePresentModesKHR(ae::VulkanManager::Get().GetPhysicalDevice(), m_Surface,
                                              &presentModeCount, nullptr);

    std::vector<VkPresentModeKHR> presentModes(presentModeCount);

    vkGetPhysicalDeviceSurfacePresentModesKHR(ae::VulkanManager::Get().GetPhysicalDevice(), m_Surface,
                                              &presentModeCount, presentModes.data());

    VkPresentModeKHR chosenPresentMode = VK_PRESENT_MODE_FIFO_KHR; // guaranteed

    for (const auto &mode : presentModes)
    {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            chosenPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
            break;
        }
    }

    if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        m_SwapChainExtent = surfaceCapabilities.currentExtent;
    }

    else
    {
        int width;
        int height;
        glfwGetFramebufferSize(m_Window.GetWindow(), &width, &height);

        m_SwapChainExtent.width = std::clamp((uint32_t)width, surfaceCapabilities.minImageExtent.width,
                                             surfaceCapabilities.maxImageExtent.width);

        m_SwapChainExtent.height = std::clamp((uint32_t)height, surfaceCapabilities.minImageExtent.height,
                                              surfaceCapabilities.maxImageExtent.height);
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_Surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = m_SwapChainImageFormat;
    createInfo.imageColorSpace = chosenFormat.colorSpace;
    createInfo.imageExtent = m_SwapChainExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

    createInfo.preTransform = surfaceCapabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = chosenPresentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(VulkanManager::Get().GetDevice(), &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS)
    {
        AE_THROW_RUNTIME_ERROR("Failed to create swap chain for Vulkan context");
    }

    uint32_t swapImageCount;
    vkGetSwapchainImagesKHR(VulkanManager::Get().GetDevice(), m_SwapChain, &swapImageCount, nullptr);

    m_SwapChainImages.resize(swapImageCount);
    vkGetSwapchainImagesKHR(VulkanManager::Get().GetDevice(), m_SwapChain, &swapImageCount, m_SwapChainImages.data());
}

void ae::VulkanContext::DestroySwapChain()
{
    vkDestroySwapchainKHR(VulkanManager::Get().GetDevice(), m_SwapChain, nullptr);
    m_SwapChain = VK_NULL_HANDLE;
    m_SwapChainImages.clear();
}

void ae::VulkanContext::CreateSwapChainImageViews()
{
    m_SwapChainImageViews.resize(m_SwapChainImages.size());

    for (size_t i = 0; i < m_SwapChainImages.size(); i++)
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = m_SwapChainImages[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = m_SwapChainImageFormat;
        viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(VulkanManager::Get().GetDevice(), &viewInfo, nullptr, &m_SwapChainImageViews[i]) !=
            VK_SUCCESS)
        {
            AE_THROW_RUNTIME_ERROR("Failed to create image views for Vulkan context");
        }
    }
}

void ae::VulkanContext::DestroySwapChainImageViews()
{
    for (auto *imageView : m_SwapChainImageViews)
    {
        vkDestroyImageView(VulkanManager::Get().GetDevice(), imageView, nullptr);
    }

    m_SwapChainImageViews.clear();
}

void ae::VulkanContext::CreateImGuiStandaloneRenderPass()
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = m_SwapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(VulkanManager::Get().GetDevice(), &renderPassInfo, nullptr,
                           &m_ImGuiStandaloneRenderPass) != VK_SUCCESS)
    {
        AE_THROW_RUNTIME_ERROR("Failed to create ImGui standalone render pass");
    }
}

void ae::VulkanContext::DestroyImGuiStandaloneRenderPass()
{
    vkDestroyRenderPass(VulkanManager::Get().GetDevice(), m_ImGuiStandaloneRenderPass, nullptr);
    m_ImGuiStandaloneRenderPass = VK_NULL_HANDLE;
}

void ae::VulkanContext::CreateImGuiOverlayRenderPass()
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = m_SwapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(VulkanManager::Get().GetDevice(), &renderPassInfo, nullptr,
                           &m_ImGuiOverlayRenderPass) != VK_SUCCESS)
    {
        AE_THROW_RUNTIME_ERROR("Failed to create ImGui overlay render pass");
    }
}

void ae::VulkanContext::DestroyImGuiOverlayRenderPass()
{
    vkDestroyRenderPass(VulkanManager::Get().GetDevice(), m_ImGuiOverlayRenderPass, nullptr);
    m_ImGuiOverlayRenderPass = VK_NULL_HANDLE;
}

void ae::VulkanContext::CreateImGuiStandaloneFramebuffers()
{
    m_ImGuiStandaloneFramebuffers.resize(m_SwapChainImageViews.size());

    for (size_t i = 0; i < m_SwapChainImageViews.size(); i++)
    {
        std::array<VkImageView, 1> attachments = { m_SwapChainImageViews[i] };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_ImGuiStandaloneRenderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = m_SwapChainExtent.width;
        framebufferInfo.height = m_SwapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(VulkanManager::Get().GetDevice(), &framebufferInfo, nullptr,
                                &m_ImGuiStandaloneFramebuffers[i]) != VK_SUCCESS)
        {
            AE_THROW_RUNTIME_ERROR("Failed to create ImGui standalone framebuffer");
        }
    }
}

void ae::VulkanContext::DestroyImGuiStandaloneFramebuffers()
{
    for (auto *framebuffer : m_ImGuiStandaloneFramebuffers)
    {
        vkDestroyFramebuffer(VulkanManager::Get().GetDevice(), framebuffer, nullptr);
    }
    m_ImGuiStandaloneFramebuffers.clear();
}

void ae::VulkanContext::CreateImGuiOverlayFramebuffers()
{
    m_ImGuiOverlayFramebuffers.resize(m_SwapChainImageViews.size());

    for (size_t i = 0; i < m_SwapChainImageViews.size(); i++)
    {
        std::array<VkImageView, 1> attachments = { m_SwapChainImageViews[i] };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_ImGuiOverlayRenderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = m_SwapChainExtent.width;
        framebufferInfo.height = m_SwapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(VulkanManager::Get().GetDevice(), &framebufferInfo, nullptr,
                                &m_ImGuiOverlayFramebuffers[i]) != VK_SUCCESS)
        {
            AE_THROW_RUNTIME_ERROR("Failed to create ImGui overlay framebuffer");
        }
    }
}

void ae::VulkanContext::DestroyImGuiOverlayFramebuffers()
{
    for (auto *framebuffer : m_ImGuiOverlayFramebuffers)
    {
        vkDestroyFramebuffer(VulkanManager::Get().GetDevice(), framebuffer, nullptr);
    }
    m_ImGuiOverlayFramebuffers.clear();
}

void ae::VulkanContext::CreateCommandPool()
{
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = VulkanManager::Get().GetGraphicsQueueFamilyIndex();
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(VulkanManager::Get().GetDevice(), &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS)
    {
        AE_THROW_RUNTIME_ERROR("Failed to create command pool for Vulkan context");
    }
}

void ae::VulkanContext::DestroyCommandPool()
{
    vkDestroyCommandPool(VulkanManager::Get().GetDevice(), m_CommandPool, nullptr);
    m_CommandPool = VK_NULL_HANDLE;
}

void ae::VulkanContext::CreateCommandBuffers()
{
    size_t imageCount = m_SwapChainImages.size();

    // ImGui command buffers (one per swapchain image)
    m_ImGuiCommandBuffers.resize(imageCount);
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_CommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(imageCount);

    if (vkAllocateCommandBuffers(VulkanManager::Get().GetDevice(), &allocInfo, m_ImGuiCommandBuffers.data()) !=
        VK_SUCCESS)
    {
        AE_THROW_RUNTIME_ERROR("Failed to allocate ImGui command buffers");
    }

    // Transition command buffers (one per swapchain image)
    m_TransitionCommandBuffers.resize(imageCount);
    allocInfo.commandBufferCount = static_cast<uint32_t>(imageCount);

    if (vkAllocateCommandBuffers(VulkanManager::Get().GetDevice(), &allocInfo, m_TransitionCommandBuffers.data()) !=
        VK_SUCCESS)
    {
        AE_THROW_RUNTIME_ERROR("Failed to allocate transition command buffers");
    }
}

void ae::VulkanContext::DestroyCommandBuffers()
{
    if (!m_ImGuiCommandBuffers.empty())
    {
        vkFreeCommandBuffers(VulkanManager::Get().GetDevice(), m_CommandPool,
                             static_cast<uint32_t>(m_ImGuiCommandBuffers.size()), m_ImGuiCommandBuffers.data());
        m_ImGuiCommandBuffers.clear();
    }

    if (!m_TransitionCommandBuffers.empty())
    {
        vkFreeCommandBuffers(VulkanManager::Get().GetDevice(), m_CommandPool,
                             static_cast<uint32_t>(m_TransitionCommandBuffers.size()),
                             m_TransitionCommandBuffers.data());
        m_TransitionCommandBuffers.clear();
    }
}

void ae::VulkanContext::CreateSyncObjects()
{
    m_ImageAvailableSemaphores.resize(m_FramesInFlight);
    m_RenderFinishedSemaphores.resize(m_FramesInFlight);
    m_InFlightFences.resize(m_FramesInFlight);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (uint32_t i = 0; i < m_FramesInFlight; i++)
    {
        if (vkCreateSemaphore(VulkanManager::Get().GetDevice(), &semaphoreInfo, nullptr,
                              &m_ImageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(VulkanManager::Get().GetDevice(), &semaphoreInfo, nullptr,
                              &m_RenderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(VulkanManager::Get().GetDevice(), &fenceInfo, nullptr, &m_InFlightFences[i]) != VK_SUCCESS)
        {
            AE_THROW_RUNTIME_ERROR("Failed to create synchronization objects for Vulkan context");
        }
    }
}

void ae::VulkanContext::DestroySyncObjects()
{
    for (uint32_t i = 0; i < m_InFlightFences.size(); i++)
    {
        vkDestroySemaphore(VulkanManager::Get().GetDevice(), m_ImageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(VulkanManager::Get().GetDevice(), m_RenderFinishedSemaphores[i], nullptr);
        vkDestroyFence(VulkanManager::Get().GetDevice(), m_InFlightFences[i], nullptr);
    }

    m_ImageAvailableSemaphores.clear();
    m_RenderFinishedSemaphores.clear();
    m_InFlightFences.clear();
}

void ae::VulkanContext::RecreateSwapChain()
{
    vkDeviceWaitIdle(VulkanManager::Get().GetDevice());

    DestroySyncObjects();
    DestroyCommandBuffers();
    DestroyImGuiOverlayFramebuffers();
    DestroyImGuiStandaloneFramebuffers();
    DestroyImGuiOverlayRenderPass();
    DestroyImGuiStandaloneRenderPass();
    DestroySwapChainImageViews();
    DestroySwapChain();

    CreateSwapChain();
    CreateSwapChainImageViews();
    CreateImGuiStandaloneRenderPass();
    CreateImGuiOverlayRenderPass();
    CreateImGuiStandaloneFramebuffers();
    CreateImGuiOverlayFramebuffers();
    CreateCommandBuffers();
    CreateSyncObjects();

    m_CurrentFrame = 0;

    if (m_OnSwapchainRecreated)
    {
        m_OnSwapchainRecreated(GetVulkanResources());
    }
}

void ae::VulkanContext::OnResize(uint32_t width, uint32_t height)
{
    if (width == 0 || height == 0)
    {
        return;
    }

    m_NeedsResize = true;
}

void ae::VulkanContext::SetOnSwapchainRecreatedCB(const std::function<void(const VulkanResources &)> &cb)
{
    m_OnSwapchainRecreated = cb;
}

ae::VulkanResources ae::VulkanContext::GetVulkanResources() const
{
    VulkanResources resources{};
    resources.instance = ae::VulkanManager::Get().GetInstance();
    resources.physicalDevice = ae::VulkanManager::Get().GetPhysicalDevice();
    resources.device = ae::VulkanManager::Get().GetDevice();
    resources.graphicsQueue = ae::VulkanManager::Get().GetGraphicsQueue();
    resources.graphicsQueueFamilyIndex = ae::VulkanManager::Get().GetGraphicsQueueFamilyIndex();

    resources.swapchainFormat = m_SwapChainImageFormat;
    resources.swapchainExtent = m_SwapChainExtent;
    resources.imageCount = static_cast<uint32_t>(m_SwapChainImages.size());
    resources.swapchainImageViews = m_SwapChainImageViews.data();

    return resources;
}

#endif // AE_VULKAN
