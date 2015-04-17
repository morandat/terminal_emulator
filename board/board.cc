#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>

#include "Socket.h"

#include "board.h"

#define BUTTON_SIZE 128
#define WINDOW_WIDTH 1146
#define WINDOW_HEIGHT 512
#define WINDOW_MARGIN 5
#define LINE_HEIGHT 22

#define BUFFER_SIZE 1024
#define WINDOW_NAME "Image Window"
#define FONTFACE cv::FONT_HERSHEY_DUPLEX

static char const *error_msgs[] = {
    "Invalid command",
    "Invalid index"
};

class Screen {
    cv::Mat& image;

    int b_states;
    int p_buttons;
    char *display[SCREEN_LINES];
    char *b_labels[BUTTONS_COUNT];
    std::vector<char> l_keys;

    public: 
    Screen(cv::Mat& image): image(image) {
        b_states = p_buttons = 0;
        for(int i = 0; i < BUTTONS_COUNT; i++) {
            b_labels[i] = (char*)memset(new char[BUTTON_TEXT_LEN + 1], 0, BUTTON_TEXT_LEN + 1);
            sprintf(b_labels[i], "Button %x", i + 1);
        }

        for(int i = 0; i < SCREEN_LINES; i++){
            display[i] = (char*)memset(new char[SCREEN_COLUMNS + 1], 0, SCREEN_COLUMNS + 1);
            for (int j = 0; j < SCREEN_COLUMNS; j ++)
                display[i][j] = ' ' + i + j;
        }

        cv::setMouseCallback(WINDOW_NAME, (void (*)(int, int, int, int, void*))mouseAdapter, this);
    }

    virtual ~Screen() {
        for(int i = 0; i < SCREEN_LINES; i++){
            delete [] display[i];
        }
    }

    // return true if we should exit
    bool processEvents(int delay) {
        int c = cvWaitKey(1);
        if( c != -1 ) {
            if (c == 27) {
                return true;
            } else if ((c & 0xf700) == 0xf700) {
                c &= 0xff;
                if (c >= 4 && c <= 0xb) // F1 ... F4 .. F8
                    pushButton(c - 4);
                else if(c >= 0xc && c <= 0xf) // F9 .. F12
                    pushButton(c - 8);
            } else if (c & 0xff)
                pressKey(c & 0xff);
        }
        return false;
    }

    void drawScreen() {
        clearScreen(image);
        for (int i = 0; i < BUTTONS_COUNT; i ++) {
            drawRectangle(image, b_labels[i], !!(b_states & (1 << i)),
                    i / 4 * (WINDOW_WIDTH - BUTTON_SIZE), (i % 4) * BUTTON_SIZE, BUTTON_SIZE, BUTTON_SIZE);
        }

        for(int i = 0; i < SCREEN_LINES; i++)
            setLabel(image, BUTTON_SIZE + WINDOW_MARGIN, LINE_HEIGHT * (i + 1), display[i]);

        cv::imshow(WINDOW_NAME, image);
    }

    bool pushButton(int button) {
        if (button < 0)
            return false;
        p_buttons |= 1 << button;
        return true;
    }

    bool setLine(int line, char *text) {
        if (line >= 0 && line < SCREEN_LINES) {
            strncpy(display[line],  text, SCREEN_COLUMNS);
            return true;
        }
        return false;
    }

    int setButtonText(int btn, char *text) {
        if (btn >= BUTTONS_COUNT)
            return false;
        strncpy(b_labels[btn], text, BUTTON_TEXT_LEN);
        return true;
    }

    void setButtonStates(int state) {
        b_states = state & ((1 << BUTTONS_COUNT) - 1);
    }

    int getButton() {
        return p_buttons;
    }

    void pressKey(int key) {
        l_keys.push_back(key);
    }

    int getKey() {
        if( l_keys.size() > 0 ){
            char answ = l_keys.at( 0 );
            l_keys.erase( l_keys.begin() );
            return answ;
        }
        return 0;
    }

    void releaseDisplay() {
        cvDestroyWindow(WINDOW_NAME);
    }

    static void mouseAdapter(int event, int x, int y, int flags, Screen* screen) {
        screen->mouseClicked(event, x, y, flags);
    }

    void mouseClicked(int event, int x, int y, int flags) {
        if (event == CV_EVENT_LBUTTONDOWN) {
            int button = -1;
            if (x < BUTTON_SIZE) { 
                button = (y / BUTTON_SIZE);
            } else if (x > (WINDOW_WIDTH - BUTTON_SIZE)) {
                button = (y / BUTTON_SIZE) + 4;
            }
            pushButton(button);
        }
    }

    protected:

    // Helper function to put text in the center of a rectangle
    void setCenteredLabel(cv::Mat& im, cv::Rect r, const std::string label, CvScalar color = CV_RGB(255, 255, 255), int fontface = FONTFACE) {
        double scale = 0.7;
        int baseline;
        cv::Size text = cv::getTextSize(label, FONTFACE, scale, 1, &baseline);
        cv::Point pt(r.x + (r.width-text.width)/2, r.y + (r.height+text.height)/2);
        setLabel(im, pt, label, scale, color, fontface);
    }

    void setLabel(cv::Mat& im, int x, int y, const std::string label, CvScalar color = CV_RGB(255, 255, 255), int fontface = FONTFACE) {
        setLabel(im, cvPoint(x,y), label, 0.55, color, fontface);
    }

    void setLabel(cv::Mat& im, CvPoint pt, const std::string label, double scale, CvScalar color, int fontface) {
        cv::putText(im, label, pt, FONTFACE, scale, color, 1, CV_AA);
    }

    void drawRectangle(cv::Mat& im, const std::string texte, bool state, int x, int y, int w, int h)
    {
        CvScalar white = CV_RGB( 255, 255, 255 );
        CvScalar black = CV_RGB(   0,   0,   0 );
        CvRect  rect   = cvRect( x, y, w, h );
        if( state == true ){
            cv::rectangle(im, rect, white, 1, 8, 0);
            setCenteredLabel(im, rect, texte, white);
        }else{
            cv::rectangle(im, rect, white, 1, 8, 0);
            setCenteredLabel(im, rect, texte, black);
        }
    }

    void clearScreen(cv::Mat& im){
        CvScalar black = CV_RGB( 0, 0, 0 );
        CvRect rect = cvRect( 0, 0, im.cols, im.rows );
        cv::rectangle(im, rect, black, CV_FILLED, 8, 0);
    }
};

class ScreenHandler {
    Socket& socket;
    Screen& screen;
    char buffer[BUFFER_SIZE];

    public:
    ScreenHandler(Socket& sock, Screen& scr) : socket(sock), screen(scr) {
    }

    int handleRequest() {
        int l = socket.receive(buffer, 1024);
        switch (l) {
            case -1: // EAGAIN
                return 0;
            case 0: // EOF
                return -1;
            default:
                try {
                    int result = processRequest(buffer);
                    sendReply(result);
                    return result != 0;
                } catch(...) {
                    return -1;
                }
        }
    }

    protected:
    int processRequest(const char *request) {
        char *t;
        switch ((request = getRequest(request)) ? *request: 0) {
            case 'l':
            case 'L': {
                          int line = strtol(stripSpaces(request + 1), &t, 16);
                          *(char *)eos(t + 1) = 0; // ensure is terminated well (and yes that is the worst cast ever)
                          return screen.setLine(line, t + 1) ? 1 : -2;
                      }
                      return  1;
            case 'c':
            case 'C':
                      screen.setButtonStates(strtol(stripSpaces(request + 1), NULL, 16));
                      return 1;
            case 't': 
            case 'T': {
                          int btn = strtol(stripSpaces(request + 1), &t, 16);
                          *(char *)eos(t + 1) = 0; // ensure is terminated well (and yes that is the worst cast ever)
                          return screen.setButtonText(btn, t + 1);
                      }
            case 'b': 
            case 'B':
                      return screen.getButton();
            case 'k': 
            case 'K': 
                      return screen.getKey(); // 0 <=> no key pressed
            case 'q':
            case 'Q':
                      socket.close();
                      throw "client disconnected.";
        }
        return -1;
    }

    void sendReply(int code) {
        int len;
        if (code < 0) {
            int idx = - code - 1;
            if (idx > (ssize_t)(sizeof(error_msgs)/(sizeof(error_msgs[0]))))
                idx = 0;
            len = snprintf(buffer, BUFFER_SIZE, KO_STRING ":%s" CRLF, error_msgs[idx]);
            //  } else if (code == 0){
            //      len = snprintf(buffer, BUFFER_SIZE, "#OK\r\n");
        } else 
            len = snprintf(buffer, BUFFER_SIZE, OK_STRING ":%2.2x" CRLF, code);

        if (len > BUFFER_SIZE)
            len = BUFFER_SIZE;

        socket.send(buffer, len);
        if (code < 0)
            std::cerr << buffer << std::endl;
    }

    private:
    // Next non whitespace char
    const char *stripSpaces(const char *p) {
        while (*p == ' ') p ++;
        return p;
    }

    // End of String (i.e., 0x00, 0x0a, 0x0c)
    const char *eos(char *p) {
        while(1) {
            switch(*p) {
                case 0:
                case '\n':
                case '\r':
                    return p;
            }
            p ++;
        }
    }

    // Return a pointer to the request if it's valid, null otherwise
    const char *getRequest(const char *buffer) {
        const char *p = buffer;
        if (*p != '#')
            return NULL;
        return stripSpaces(p + 1);
    }
};

cv::Mat* initFrame() {
    cvNamedWindow( WINDOW_NAME, CV_WINDOW_AUTOSIZE );
    return new cv::Mat(cvCreateImage(cvSize(WINDOW_WIDTH, WINDOW_HEIGHT), IPL_DEPTH_8U , 3));
}

int main( int argc, char** argv ) {
    int port = (argc > 1) ? atoi(argv[1]) : DEFAULT_PORT;
    ServerSocket server(port);

    std::cout << "(II) Creation of the window" <<  std::endl;
    Screen screen(*initFrame());
    screen.drawScreen();
    std::cout << "(II) Waiting for client connection (" << port << ")" << std::endl;
    Socket& sock = server.accept();
    std::cout << "(II) Client connection accepted" << std::endl;
    ScreenHandler hdl(sock, screen);

    bool leave   = false;

    while(!leave) {
        bool updated = false;
        bool processedEvents = true;
        while (processedEvents) {
            switch(hdl.handleRequest()) {
                case 1:
                    updated = true;
                    break;
                case -1:
                    std::perror("End of connection");
                    leave = true;
                case 0:
                    processedEvents = false;
            }
        }
        if (updated)
            screen.drawScreen();
        if (screen.processEvents(1)) {
            leave = true;
        }
    }

    server.close();
    screen.releaseDisplay();
    return 0;
}
