#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>

#include "Socket.h"

#define DEFAULT_PORT 2002
#define SCREEN_LINES 24
#define SCREEN_COLUMNS 80
#define BUTTONS_COUNT 8

#define BUTTON_SIZE 128
#define WINDOW_WIDTH 1146
#define WINDOW_HEIGHT 512
#define WINDOW_MARGIN 5
#define LINE_HEIGHT 22

#define BUFFER_SIZE 1024
#define WINDOW_NAME "Image Window"

static char const *error_msgs[] = {
    "Invalid command",
    "Invalid index"
};

class Screen {
  cv::Mat& image;

  bool b_states[BUTTONS_COUNT];
  std::vector<std::string> b_labels;
  std::vector<int>  p_buttons;
  std::vector<char> l_touches;
  std::vector<std::string*> display;

  public: 
  Screen(cv::Mat& image): image(image) {
    char b[] = "Button X";
    for(int i = 0; i < BUTTONS_COUNT; i++) {
      b_states[i] = false;
      b[7] = '1' + i;
      b_labels.push_back(b);
    }

    for(int i=0; i< SCREEN_LINES; i++){
        std::string* str = new std::string();
        (*str) = "";
        for(int j = 0; j < 80; j++)
          (*str) = (*str) + (char)('0' + rand()%2);
        display.push_back( str );
    }

    cv::setMouseCallback(WINDOW_NAME, (void (*)(int, int, int, int, void*))mouseAdapter, this);
  }

  bool shouldExit(int delay) {
    int c = cvWaitKey(1);
    if( c != -1 ){
      if (c == 27) {
        return true;
      } else
        pressKey(c);
    }
    return false;
  }

  void drawScreen() {
    clearScreen(image);
    for (int i = 0; i < BUTTONS_COUNT; i ++) {
      drawRectangle(image, b_labels[i], b_states[i],
          i / 4 * (WINDOW_WIDTH - BUTTON_SIZE), (i % 4) * BUTTON_SIZE, BUTTON_SIZE, BUTTON_SIZE);
    }

    for(int i = 0; i < SCREEN_LINES; i++)
      setLabel(image, BUTTON_SIZE + WINDOW_MARGIN, LINE_HEIGHT * i, *display.at(i));

    cv::imshow(WINDOW_NAME, image);
  }

  void pushButton(int button) {
    if( (button != 0) && (b_states[button-1] != 0) ){
      p_buttons.push_back( button );
    }
  }

  void setLine(int line, char *text) {// TODO return error
    if (line > 0 && line < SCREEN_LINES) {
      *display[line] = text;
    }
  }

  void setButton(int btn, int state) { // TODO return error
    if (btn >= 0 && btn < BUTTONS_COUNT)
      b_states[btn] = state & 0x01;
  }

  int getButton() {
    if (p_buttons.size() > 0) {
      char answ = p_buttons.at(0);
      p_buttons.erase(p_buttons.begin());
      return answ;
    }
    return 0;
  }

  void pressKey(int key) {
    l_touches.push_back(key);
  }

  int getKey() {
    if( l_touches.size() > 0 ){
      char answ = l_touches.at( 0 );
      l_touches.erase( l_touches.begin() );
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
      std::cout << "MOUSE DOWN => posX = " << x << " and posY = " << y << std::endl;
      int button = 0;
      if (x < BUTTON_SIZE) { 
        button = (y / BUTTON_SIZE) + 1;
      } else if (x > (WINDOW_WIDTH - BUTTON_SIZE)) {
        button = (y / BUTTON_SIZE) + 5;
      }
      std::cout << " > button = " << button << std::endl;
      pushButton(button);
    }
  }

  protected:

  // Helper function to put text in the center of a rectangle
  void setLabel(cv::Mat& im, cv::Rect r, const std::string label, CvScalar color) {
    int fontface = cv::FONT_HERSHEY_SIMPLEX;
    double scale = 0.7;
    int thickness = 1;
    int baseline = 0;

    cv::Size text = cv::getTextSize(label, fontface, scale, thickness, &baseline);
    cv::Point pt(r.x + (r.width-text.width)/2, r.y + (r.height+text.height)/2);

    int face[] = {cv::FONT_HERSHEY_SIMPLEX, cv::FONT_HERSHEY_PLAIN, cv::FONT_HERSHEY_DUPLEX, cv::FONT_HERSHEY_COMPLEX, 
      cv::FONT_HERSHEY_TRIPLEX, cv::FONT_HERSHEY_COMPLEX_SMALL, cv::FONT_HERSHEY_SCRIPT_SIMPLEX, 
      cv::FONT_HERSHEY_SCRIPT_COMPLEX, cv::FONT_ITALIC};
    cv::putText(im, label, pt, face[0]|face[8]/*fontface*/, scale, color, thickness, CV_AA);
  }

  void setLabel(cv::Mat& im, int x, int y, const std::string label, CvScalar color = CV_RGB( 255, 255, 255 )) {
    int fontface  = cv::FONT_HERSHEY_SIMPLEX;
    double scale  = 0.55;
    int thickness = 1;
    int baseline  = 0;
    CvPoint pt    = cvPoint(x, y);
    //    cv::Point pt(r.x + (r.width-text.width)/2, r.y + (r.height+text.height)/2);
    int face[] = {cv::FONT_HERSHEY_SIMPLEX, cv::FONT_HERSHEY_PLAIN, cv::FONT_HERSHEY_DUPLEX, cv::FONT_HERSHEY_COMPLEX, 
      cv::FONT_HERSHEY_TRIPLEX, cv::FONT_HERSHEY_COMPLEX_SMALL, cv::FONT_HERSHEY_SCRIPT_SIMPLEX, 
      cv::FONT_HERSHEY_SCRIPT_COMPLEX, cv::FONT_ITALIC};
    cv::putText(im, label, pt, face[0]/*fontface*/, scale, color, thickness, CV_AA);
  }

  void drawRectangle(cv::Mat& im, const std::string texte, bool state, int x, int y, int w, int h)
  {
    CvScalar white = CV_RGB( 255, 255, 255 );
    CvScalar black = CV_RGB(   0,   0,   0 );
    CvRect  rect   = cvRect( x, y, w, h );
    if( state == true ){
      cv::rectangle(im, rect, white, 1, 8, 0);
      setLabel(im, rect, texte, white);
    }else{
      cv::rectangle(im, rect, white, 1, 8, 0);
      setLabel(im, rect, texte, black);
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

  bool handleRequest() {
    std::string t_recv = socket.Receive();

    if( t_recv.length() > 0 ) {
      int result = processRequest(t_recv.c_str());
      sendReply(result);
      return true;
    }
    return false;
  }

  protected:
  int processRequest(const char *request) {
    char *t;
    switch ((request = getRequest(request)) ? *request: 0) {
      case 'l':
      case 'L': {
                  int line = strtol(stripSpaces(request + 1), &t, 10);
                  *(char *)eos(t + 1) = 0; // ensure is terminated well (and yes that is the worst cast ever)
                  if (!(line > 0 && line < SCREEN_LINES))
                    return -2;
                  screen.setLine(line, t + 1);
                  return  0;
                }
      case 'c':
      case 'C': {
                  int btn = strtol(stripSpaces(request + 1), &t, 10);
                  int col = strtol(request = t + 1, &t, 10);
                  if (btn <= 0 || btn > BUTTONS_COUNT)
                    return -2;
                  screen.setButton(btn - 1, col);
                  return 0;
                }
      case 'b': 
      case 'B':
                  return screen.getButton();
      case 'k': 
      case 'K': 
                return screen.getKey(); // 0 <=> no key pressed
    }
    return -1;
  }

  void sendReply(int code) {
    int len;
    if (code < 0) {
      int idx = - code - 1;
      if (idx > (sizeof(error_msgs)/(sizeof(error_msgs[0]))))
        idx = 0;
      len = snprintf(buffer, BUFFER_SIZE, "#KO:%s\r\n", error_msgs[idx]);
    } else if (code == 0){
      len = snprintf(buffer, BUFFER_SIZE, "#OK\r\n");
    } else 
      snprintf(buffer, BUFFER_SIZE, "#OK:%d\r\n", code);

    if (len > BUFFER_SIZE)
      len = BUFFER_SIZE;

    socket.Send(buffer, len);
    if (code < 0)
      std::cerr << buffer << std::endl;
    else
      std::cout << buffer << std::endl;
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
    std::cout << "(II) CREATION OF THE WINDOW" <<  std::endl;
    cvNamedWindow( WINDOW_NAME, CV_WINDOW_AUTOSIZE );
    return new cv::Mat(cvCreateImage(cvSize(WINDOW_WIDTH, WINDOW_HEIGHT), IPL_DEPTH_8U , 3));
}

int main( int argc, char** argv ) {
  int port = (argc > 1) ? atoi(argv[1]) : DEFAULT_PORT;
  Socket sock(port);
  cv::Mat& image = *initFrame();
  Screen screen(image);
  ScreenHandler hdl(sock, screen);

  bool leave   = false;

  screen.drawScreen();
  while(!leave) {
    for (int qq=0; qq<100; qq++)
    {
      if (screen.shouldExit(1)) {
        leave = true;
        break;
      }

      if (hdl.handleRequest())
        screen.drawScreen();
    }
  }

  screen.releaseDisplay();
  return 0;
}
