/*
 * Copyright (c) 2015 Nikolay Zapolnov (zapolnov@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once
#include "GLResourceManager.h"
#include "utility/FileSystemList.h"
#include "input/KeyCode.h"
#include <memory>
#include <unordered_set>
#include <unordered_map>

#define Z_GL_APPLICATION_CLASS(CLASS) \
    static_assert(std::is_base_of<::Z::GLApplication, CLASS>::value, \
        #CLASS " is not a descendant of Z::GLApplication."); \
    Z::GLApplicationPtr Z::GLApplication::create() { return std::make_shared<CLASS>(); }

namespace Z
{
    class GLApplication;
    using GLApplicationPtr = std::shared_ptr<GLApplication>;

    class GLApplication
    {
    public:
        GLApplication();
        virtual ~GLApplication() = default;

        static GLApplicationPtr create();

        const FileSystemListPtr& fileSystems() const { return m_FileSystemList; }

        GLResourceManager& resourceManager() { return m_ResourceManager; }
        const GLResourceManager& resourceManager() const { return m_ResourceManager; }

        int viewportWidth() const { return m_ViewportWidth; }
        int viewportHeight() const { return m_ViewportHeight; }

        virtual int preferredScreenWidth() const { return 1024; }
        virtual int preferredScreenHeight() const { return 768; }
        virtual int preferredDepthBufferBits() const { return 16; }
        virtual int preferredStencilBufferBits() const { return 0; }

        void initializeGL(int width, int height);
        void shutdownGL();
        void suspendGL();
        void resumeGL();

        void resizeGL(int width, int height);
        void renderGL(double time);

        void onPointerPressed(int id, float x, float y);
        void onPointerMoved(int id, float x, float y);
        void onPointerReleased(int id, float x, float y);
        void onPointerCancelled(int id, float x, float y);

        bool isKeyPressed(KeyCode key) const;
        void onKeyPressed(KeyCode key);
        void onKeyReleased(KeyCode key);

        void cancelAllTouches();
        void cancelAllPressedKeys();

    protected:
        virtual void onApplicationDidFinishLaunching() {}
        virtual void onApplicationWillTerminate() {}
        virtual void onApplicationWillEnterBackground() {}
        virtual void onApplicationDidEnterForeground() {}
        virtual void onApplicationViewportChanged() {}
        virtual void onApplicationWillReloadResources() {}

        virtual void onTouchBegan(int id, float x, float y) { (void)id; (void)x; (void)y; }
        virtual void onTouchMoved(int id, float x, float y) { (void)id; (void)x; (void)y; }
        virtual void onTouchEnded(int id, float x, float y) { (void)id; (void)x; (void)y; }
        virtual void onTouchCancelled(int id, float x, float y) { (void)id; (void)x; (void)y; }

        virtual void onKeyDown(KeyCode key) { (void)key; }
        virtual void onKeyRepeat(KeyCode key) { (void)key; }
        virtual void onKeyUp(KeyCode key) { (void)key; }

        virtual void drawResourceReloadScreen(double time, float progress);
        virtual void runFrame(double time) = 0;

    private:
        FileSystemListPtr m_FileSystemList;
        GLResourceManager m_ResourceManager;
        bool m_ReloadingResources = false;
        bool m_Suspended = false;
        int m_ViewportWidth = 0;
        int m_ViewportHeight = 0;
        std::unordered_map<int, std::pair<float, float>> m_PressedPointers;
        std::unordered_set<KeyCode> m_PressedKeys;
    };
}
