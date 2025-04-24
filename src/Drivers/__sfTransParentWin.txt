//

//
// void updateLayeredWindow(HWND hwnd, const sf::Image& image) {
//     HDC screenDC = GetDC(NULL);
//     HDC memDC = CreateCompatibleDC(screenDC);

//     int width = image.getSize().x;
//     int height = image.getSize().y;

//     BITMAPINFO bmi = {};
//     bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
//     bmi.bmiHeader.biWidth = width;
//     bmi.bmiHeader.biHeight = -height; // top-down DIB
//     bmi.bmiHeader.biPlanes = 1;
//     bmi.bmiHeader.biBitCount = 32;
//     bmi.bmiHeader.biCompression = BI_RGB;

//     void* bits = nullptr;
//     HBITMAP hBitmap = CreateDIBSection(memDC, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
//     memcpy(bits, image.getPixelsPtr(), width * height * 4);

//     HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, hBitmap);

//     POINT ptSrc = { 0, 0 };
//     SIZE sizeWnd = { width, height };
//     POINT ptDst = { 100, 100 };

//     BLENDFUNCTION blend = {};
//     blend.BlendOp = AC_SRC_OVER;
//     blend.SourceConstantAlpha = 255;
//     blend.AlphaFormat = AC_SRC_ALPHA;

//     UpdateLayeredWindow(hwnd, screenDC, &ptDst, &sizeWnd, memDC, &ptSrc, 0, &blend, ULW_ALPHA);

//     SelectObject(memDC, oldBitmap);
//     DeleteObject(hBitmap);
//     DeleteDC(memDC);
//     ReleaseDC(NULL, screenDC);
// }


//)
// sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "", sf::Style::None);

// HWND hwnd = window.getSystemHandle();

//
// LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
// SetWindowLong(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST);

//
// sf::RenderTexture rtex;
// rtex.create(sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height);

//
// sf::CircleShape circle(100);
// circle.setFillColor(sf::Color(255, 0, 0, 128));
// circle.setPosition(100, 100);

// MSG msg = {};
// bool running = true;
// while (running) {

//     while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
//         if (msg.message == WM_QUIT)
//             running = false;
//         TranslateMessage(&msg);
//         DispatchMessage(&msg);
//     }

//     rtex.clear(sf::Color::Transparent);
//     circle.setPosition(circle.getPosition().x + 5, circle.getPosition().y + 5);
//     rtex.draw(circle);
//     rtex.display();

//     updateLayeredWindow(hwnd, rtex.getTexture().copyToImage());

//     Sleep(16); // ~60 FPS
//     if(circle.getPosition().x > 200){
//         running = false;
//     }
// }

// window.close();