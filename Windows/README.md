Задание к занятию 6 и 7
Дедлайн: 19 апреля
Читать:
 http://msdn.microsoft.com/en-us/library/windows/desktop/ms632586(v=vs.85).aspx
http://msdn.microsoft.com/en-us/library/windows/desktop/dd145203(v=vs.85).aspx

1.  Создайте пустой C++ проект.
2.  Реализуйте пустую фукнцию WinMain.
3.  Зарегистрируйте класс overlapped-окна и напишите оконную процедуру, обрабатывающую сообщение WM_DESTROY. RegisterClassEx.
4.  Создайте одно такое окно, отобразите его и запустите цикл обработки сообщений. CreateWindowEx, ShowWindow, GetMessage, TranslateMessage, DispatchMessage.
5.  Создайте класс окна с функциями регистрации класса, создания окна и со статической оконной функцией. Запоминайте HWND окна при создании и указатель на окно в SetWindowLong.
class COverlappedWindow {
public:
        COverlappedWindow();
        ~COverlappedWindow();
        // Зарегистрировать класс окна
        static bool RegisterClass();
        // Создать экземпляр окна
        bool Create();
        // Показать окно
        void Show(int cmdShow);
protected:
        void OnDestroy();
        void OnCreate();
        void OnNCCreate( const HWND handle );
        void OnTimer();
        void OnPaint();
private:
        HWND handle; // хэндл окна
        static LRESULT __stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
};
6. Нарисуйте в окне эллипс с фоном.
Обработайте сообщение:
WM_PAINT
Используйте методы WinAPI:
BeginPaint
EndPaint
GetClientRect
SelectObject
CreateSolidBrush
Rectangle или FillRect
Ellipse
DeleteObject
7. Заставьте эллипс перемещаться по экрану по таймеру. Под перемещением по экрану подразумевается, движение с отталкиванием от стенок. Важно: эллипс при движении не должен выходить за границы окна. Для этого размер можно зафиксировать (SetWindowPos). Однако, рекомендуется писать код так, чтобы быть готовым обрабатывать сообщение WM_SIZE в следующем задании.
Обработайте сообщение:
WM_TIMER
Используйте методы WinAPI:
SetTimer
KillTimer
InvalidateRect
8. Обеспечьте плавную отрисовку при помощи контекста в памяти (двойная буферизация).
Используйте методы WinAPI:
CreateCompatibleDC
CreateCompatibleBitmap
SelectObject
BitBlt
DeleteDC
                  DeteleObject
