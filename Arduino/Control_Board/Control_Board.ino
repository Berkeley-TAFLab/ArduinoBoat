#include <XBee.h>
#include <Keypad.h>
#include <UTFT.h>

#define TFT_CS 38  // Chip Select pin
#define TFT_DC 39  // Data/Command pin
#define TFT_WR 40  // Write pin
#define TFT_RST 41 // Reset pin
#define xbeeSerial Serial1

unsigned long currentTime = 0;
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
int currentDrone = 0;
int transferCheckIndex = 0;

class AnalogInput {
public:
  const int potPin1 = A0;
  const int potPin2 = A1;
  int rudder;
  int propellor;

  AnalogInput() {}

  void checkInputs() {
    int potVal1 = analogRead(potPin1);
    int potVal2 = analogRead(potPin2);
    rudder = map(potVal1, 0, 1023, 0, 180);
    propellor = map(potVal2, 0, 1023, 180, 0);
  }
};

AnalogInput analogInput;

class Drone {
public:
  char name;
  bool manual;
  bool transferData;
  int transferIndex;
  double* dataList[3];
  double control[6] = {0};
  double data[4] = {0};
  double autonomous[6] = {0};
  String MAC_ADR;
  unsigned long lastSendTime;
  unsigned long sendInterval = 200;

  Drone(char n, String adr) {
    name = n;
    manual = true;
    transferData = false;
    transferIndex = -2;
    MAC_ADR = adr;
    lastSendTime = 0;
    dataList[0] = control;
    dataList[1] = data;
    dataList[2] = autonomous;
  };

  int getDataSize(int dataType) {
  switch (dataType) {
    case 0:
      return sizeof(control) / sizeof(control[0]);
    case 1:
      return sizeof(data) / sizeof(data[0]);
    case 2:
      return sizeof(autonomous) / sizeof(autonomous[0]);
  }
    return 0;
  }

  void checkTransferData() {
    if (transferData) {
      if (transferIndex >= getDataSize(1)) {
        transferData = false;
        transferIndex = -2;
      } else {
        if (transferIndex == -2) {
          Serial.println(name + String(transferIndex + 2) + String(dataList[0][0], 6));
        } else if (transferIndex == -1) {
          Serial.println(name + String(transferIndex + 2) + String(dataList[0][1], 6));
        } else {
          Serial.println(name + String(transferIndex + 2) + String(dataList[1][transferIndex]));
        }
        transferIndex++;
        transferCheckIndex--;
      }
    }
  }
};

// Drone droneList[5] = {Drone('1', "0013A2004233185B"), Drone('2', "0013A20042331931"), Drone('3', "0013A2004233199F"), Drone('4', "0013A200423319C0"), Drone('5', "0013A20042331868")};
Drone droneList[5] = {Drone('1', "0013A2004233185B"), Drone('2', "0013A20042331931"), Drone('3', "0013A2004233199F"), Drone('4', "0013A20042331868"), Drone('5', "0013A200423319C0")};

class Coordinate {
public:
  char nodeID;
  double lat;
  double lon;

  Coordinate(char id, double la, double lo) {
    nodeID = id;
    lat = la;
    lon = lo;
  }
};

Coordinate coordinateList[9] = {Coordinate('A', 37.8671661056888, -122.31747123612297),
                                Coordinate('B', 37.867132960335766, -122.3177855855726),
                                Coordinate('C', 37.86728559529174, -122.31767266325322),
                                Coordinate('D', 37.86711367592042, -122.31770440595825), 
                                Coordinate('E', 37.86666041862518, -122.31802358417791), 
                                Coordinate('F', 37.86673349712532, -122.3172895069957),
                                Coordinate('G', 37.86750192779056, -122.3172052962821),
                                Coordinate('H', 37.83921796482649, -122.31292137919866),
                                Coordinate('I', 37.83912253830146, -122.31279207227294)
};

class Screen {
public:
  UTFT myGLCD;
  int pageTracker;
  double* command;
  int commandLength;
  bool readyToSend;

  Screen() : myGLCD(CTE40, TFT_CS, TFT_DC, TFT_WR, TFT_RST) {
    currentPage = new ControlPage(*this);
    currentMenu = mainMenu;
    commandLength = 7;
    command = new double[commandLength];
    readyToSend = false;
    pageTracker = -1;
  }

  ~Screen() {
    delete currentPage;
    delete[] command;
  }

  void start() {
    setupMenus();
    myGLCD.InitLCD();
    myGLCD.clrScr();
    myGLCD.fillScr(255, 255, 255);
    myGLCD.setBackColor(255, 255, 255);
    myGLCD.setColor(0, 0, 0);
    myGLCD.drawRect(10, 50, 230, 300);
    myGLCD.drawRect(240, 50, 470, 300);
    display();
  }

  void display() {
    if (pageTracker >= 0) {
      clearData();
      switchPage(pageTracker);
      pageTracker = -1;
    }
    currentPage->drawMenu(*this);
    currentPage->drawData(*this);
  }

  void updateData() {
    currentPage->updateData(*this);
  }

  void clearMenu() {
    myGLCD.setColor(255, 255, 255);
    myGLCD.fillRect(15, 55, 225, 295);
  }

  void clearData() {
    myGLCD.setColor(255, 255, 255);
    myGLCD.fillRect(245, 55, 465, 295);
    myGLCD.fillRect(240, 5, 465, 45);
  }

  class Page {
  public:
    int startingLine = 60;
    int lineSpacing = 18;
    int dataIndex = 250;
    int menuIndex = 20;

    double* dataArray;
    int dataLen;
    String category;
    String* dataCat;

    virtual void drawMenu(Screen& screen) {
      screen.myGLCD.setColor(0, 0, 0);
      screen.myGLCD.setFont(BigFont);
      screen.myGLCD.print("Menu", 10, 20);
      screen.myGLCD.setFont(SmallFont);
      String* options = screen.currentMenu->options;
      int optionsLen = screen.currentMenu->optionsLen;
      int linePos = startingLine;
      for (int i = 0; i < optionsLen; i++) {
        screen.myGLCD.print(String(i + 1) + ". " + options[i], menuIndex, linePos);
        linePos += lineSpacing;
      }
    }
    
    virtual void drawData(Screen& screen) {
      screen.myGLCD.setColor(0, 0, 0);
      screen.myGLCD.setFont(BigFont);
      screen.myGLCD.print(category, 240, 20);
      screen.myGLCD.setFont(SmallFont);
      int linePos = startingLine;
      screen.myGLCD.print(dataCat[0] + droneList[currentDrone].name, dataIndex, linePos);
      for (int i = 0; i < dataLen; i++) {
        linePos += lineSpacing;
        screen.myGLCD.print(dataCat[i + 1], dataIndex, linePos);
      }
      updateData(screen);
    }

    virtual void updateData(Screen& screen) {
      screen.myGLCD.setColor(0, 0, 0);
      screen.myGLCD.setFont(SmallFont);
      int linePos = startingLine + lineSpacing;
      for (int i = 0; i < dataLen; i++) {
        int index = dataCat[i + 1].length() * 8 + dataIndex;
        screen.myGLCD.printNumF(dataArray[i], 2, index, linePos);
        linePos += lineSpacing;
      }
    }

    virtual void setNewDrone() {}
    
    virtual ~Page() {}  // Virtual destructor for proper cleanup
  };

  class ControlPage : public Page {
  public:
    ControlPage(Screen& screen) {
      dataArray = droneList[currentDrone].dataList[0];
      dataLen = droneList[currentDrone].getDataSize(0);
      category = "Controls";
      dataCat = new String[8]{"Prototype: ", "Latitude: ", "Longitude: ", "Wind Angle: ", "Sail Position: ", "Rudder Position: ", "Heading: ", "Mode: "};
    }

    void setNewDrone() {
      dataArray = droneList[currentDrone].dataList[0];
      dataLen = droneList[currentDrone].getDataSize(0);
    }

    void drawData(Screen& screen) override {
      screen.myGLCD.setColor(0, 0, 0);
      screen.myGLCD.setFont(BigFont);
      screen.myGLCD.print(category, 240, 20);
      screen.myGLCD.setFont(SmallFont);
      int linePos = startingLine;
      screen.myGLCD.print(dataCat[0] + droneList[currentDrone].name, dataIndex, linePos);
      for (int i = 0; i < dataLen; i++) {
        linePos += lineSpacing;
        screen.myGLCD.print(dataCat[i + 1], dataIndex, linePos);
      }
      linePos += lineSpacing;
      String controlMode = (droneList[currentDrone].manual == 1) ? "Manual" : "Autonomous";
      screen.myGLCD.print(dataCat[7] + controlMode, dataIndex, linePos);
      updateData(screen);
    }
  };

  class DataPage : public Page {
  public:
    DataPage(Screen& screen) {
      dataArray = droneList[currentDrone].dataList[1];
      dataLen = droneList[currentDrone].getDataSize(1);
      category = "Data";
      dataCat = new String[5]{"Prototype: ", "Wind Angle: ", "Pressure: ", "Humidity: ", "Temperature: "};
    }

    void setNewDrone() {
      dataArray = droneList[currentDrone].dataList[1];
      dataLen = droneList[currentDrone].getDataSize(1);
    }
  };

  class AutonomousPage : public Page {
  public:
    AutonomousPage(Screen& screen) {
      dataArray = droneList[currentDrone].dataList[2];
      dataLen = droneList[currentDrone].getDataSize(2);
      category = "Autonomous";
      dataCat = new String[7]{"Prototype: ", "No. Waypoints: ", "Station Keeping: ", "Heading: ", "Target Bearing: ", "Distance Left: ", "SK Time: "};
    }

    void setNewDrone() {
      dataArray = droneList[currentDrone].dataList[2];
      dataLen = droneList[currentDrone].getDataSize(2);
    }
  };

  class Menu {
  public:
    Menu** nextMenus;
    Menu* prevMenu;
    String* options;
    int optionsLen;
    bool enteringCommand;

    Menu() {}

    virtual void action(int num, Screen& screen) {}
  };

  class MainMenu : public Menu {
  public:
    MainMenu(String* opt, int len, bool ec, Menu** next, Menu* prev) {
      options = opt;
      optionsLen = len;
      enteringCommand = ec;
      nextMenus = next;
      prevMenu = prev;
    }

    void action(int num, Screen& screen) override {
      if (num == 1) {
        delete screen.command;
        screen.command = new double[screen.commandLength];
        screen.command[0] = -1;
      } else if (num == 3) {
        delete screen.command;
        screen.command = new double[screen.commandLength];
        screen.command[0] = -2;
      }
    }
  };

  class SelectPrototype : public Menu {
  public:
    SelectPrototype(String* opt, int len, bool ec, Menu** next, Menu* prev) {
      options = opt;
      optionsLen = len;
      enteringCommand = ec;
      nextMenus = next;
      prevMenu = prev;
    }

    void action(int num, Screen& screen) override {
      currentDrone = num;
      screen.clearData();
      screen.currentPage->setNewDrone();
    }
  };

  class SelectControlMode : public Menu {
  public:
    SelectControlMode(String* opt, int len, bool ec, Menu** next, Menu* prev) {
      options = opt;
      optionsLen = len;
      enteringCommand = ec;
      nextMenus = next;
      prevMenu = prev;
    }

    void action(int num, Screen& screen) override {
      if (num == 0) {
        screen.command[1] = -1;
        droneList[currentDrone].manual = true;
        screen.readyToSend = true;
      } else {
        droneList[currentDrone].manual = false;
      }
    }
  };

  class SelectPage : public Menu {
  public:
    SelectPage(String* opt, int len, bool ec, Menu** next, Menu* prev) {
      options = opt;
      optionsLen = len;
      enteringCommand = ec;
      nextMenus = next;
      prevMenu = prev;
    }

    void action(int num, Screen& screen) override {
      screen.pageTracker = num;
    }
  };

  class WaypointManager : public Menu {
  public:
    WaypointManager(String* opt, int len, bool ec, Menu** next, Menu* prev) {
      options = opt;
      optionsLen = len;
      enteringCommand = ec;
      nextMenus = next;
      prevMenu = prev;
    }

    void action(int num, Screen& screen) override {
      screen.command[1] = num;
    }
  };

  class Add : public Menu {
  public:
    Add(String* opt, int len, bool ec, Menu** next, Menu* prev) {
      options = opt;
      optionsLen = len;
      enteringCommand = ec;
      nextMenus = next;
      prevMenu = prev;
    }

    void action(int num, Screen& screen) override {
      screen.command[2] = 'A' + num;
      screen.command[3] = coordinateList[num].lat;
      screen.command[4] = coordinateList[num].lon;
    }
  };

  class Loop : public Menu {
  public:
    Loop(String* opt, int len, bool ec, Menu** next, Menu* prev) {
      options = opt;
      optionsLen = len;
      enteringCommand = ec;
      nextMenus = next;
      prevMenu = prev;
    }

    void action(int num, Screen& screen) override {
      screen.command[5] = num;
    }
  };

  class StationKeep : public Menu {
  public:
    StationKeep(String* opt, int len, bool ec, Menu** next, Menu* prev) {
      options = opt;
      optionsLen = len;
      enteringCommand = ec;
      nextMenus = next;
      prevMenu = prev;
    }

    void action(int num, Screen& screen) override {
      switch (num) {
        case 0:
          screen.command[6] = 30;
        case 1:
          screen.command[6] = 60;
        case 2:
          screen.command[6] = 120;
        case 3:
          screen.command[6] = 300;
        case 4:
          screen.command[6] = 600;
        case 5:
          screen.command[6] = 1200;
      }
      screen.readyToSend = true;
      if (droneList[currentDrone].manual) {
        droneList[currentDrone].manual = false;
      }
    }
  };

  Menu* mainMenuNext[4];
  String mainMenuOptions[4] = {"Select Prototype", "Select Control Mode", "Select Page", "Waypoint Manager"};
  Menu* mainMenu = new MainMenu(mainMenuOptions, 4, false, mainMenuNext, nullptr);

  Menu* selectPrototypeNext[5];
  String selectPrototypeOptions[5] = {"1", "2", "3", "4", "5"};
  Menu* selectPrototype = new SelectPrototype(selectPrototypeOptions, 5, false, selectPrototypeNext, mainMenu);

  Menu* selectControlNext[2];
  String selectControlModeOptions[2] = {"Manual", "Autonomous"};
  Menu* selectControlMode = new SelectControlMode(selectControlModeOptions, 2, false, selectControlNext, mainMenu);

  Menu* selectPageNext[3];
  String selectPageOptions[3] = {"Controls", "Data", "Autonomous"};
  Menu* selectPage = new SelectPage(selectPageOptions, 3, false, selectPageNext, mainMenu);

  Menu* waypointManagerNext[3];
  String waypointManagerOptions[3] = {"Add", "Delete", "View"};
  Menu* waypointManager = new WaypointManager(waypointManagerOptions, 3, true, waypointManagerNext, mainMenu);

  Menu* addNext[9];
  String addOptions[9] = {"A", "B", "C", "D", "E", "F", "G", "H", "I"};
  Menu* add = new Add(addOptions, 9, true, addNext, waypointManager);

  Menu* loopNext[2];
  String loopOptions[2] = {"No Loop", "Loop"};
  Menu* loop = new Loop(loopOptions, 2, true, loopNext, add);

  Menu* stationKeepNext[6];
  String stationKeepOptions[6] = {"30 seconds", "1 minute", "2 minutes", "5 minutes", "10 minutes", "20 minutes"};
  Menu* stationKeep = new StationKeep(stationKeepOptions, 7, true, stationKeepNext, loop);

  Page* currentPage;
  Menu* currentMenu;

  void setupMenus() {
    mainMenuNext[0] = selectPrototype;
    mainMenuNext[1] = selectControlMode;
    mainMenuNext[2] = selectPage;
    mainMenuNext[3] = waypointManager;

    selectPrototypeNext[0] = mainMenu; 
    selectPrototypeNext[1] = mainMenu; 
    selectPrototypeNext[2] = mainMenu; 
    selectPrototypeNext[3] = mainMenu; 
    selectPrototypeNext[4] = mainMenu; 

    selectControlNext[0] = mainMenu;
    selectControlNext[1] = mainMenu;

    selectPageNext[0] = mainMenu;
    selectPageNext[1] = mainMenu;
    selectPageNext[2] = mainMenu;

    waypointManagerNext[0] = add;
    waypointManagerNext[1] = mainMenu;
    waypointManagerNext[2] = mainMenu;

    for (int i = 0; i < 9; i++) {
      addNext[i] = loop;
    }
    
    loopNext[0] = stationKeep;
    loopNext[1] = stationKeep;

    for (int i = 0; i < 6; i++) {
      stationKeepNext[i] = mainMenu;
    }

    mainMenu->nextMenus = mainMenuNext;
    selectPrototype->nextMenus = selectPrototypeNext;
    selectControlMode->nextMenus = selectControlNext;
    selectPage->nextMenus = selectPageNext;
    waypointManager->nextMenus = waypointManagerNext;
    add->nextMenus = addNext;
    loop->nextMenus = loopNext;
    stationKeep->nextMenus = stationKeepNext;
  }

  void switchToControlPage() {
    delete currentPage;
    currentPage = new ControlPage(*this);
  }

  void switchToDataPage() {
    delete currentPage;
    currentPage = new DataPage(*this);
  }

  void switchToAutonomousPage() {
    delete currentPage;
    currentPage = new AutonomousPage(*this);
  }

  typedef void (Screen::*PageSwitcher)();

  void switchPage(int pageIndex) {
    PageSwitcher pageSwitchers[] = {&Screen::switchToControlPage, &Screen::switchToDataPage, &Screen::switchToAutonomousPage};
    if (pageIndex >= 0 && pageIndex < 3) {
      (this->*pageSwitchers[pageIndex])();  // Call the function from the array
    }
  }
};

Screen screen;

class Input {
private:
  byte ROWS = 4;
  byte COLS = 3;
  byte rowPins[4] = {8, 7, 6, 5};
  byte colPins[3] = {4, 3, 2};
  char padArray[4][3] {
    {'1','2','3'},
    {'4','5','6'},
    {'7','8','9'},
    {'*','0','#'}
  };
public:
  Keypad keypad; 

  Input() : keypad(makeKeymap(padArray), rowPins, colPins, ROWS, COLS) {}

  void checkInput() {
    char key = keypad.getKey();
    if (key) {
      int num = key - '0' - 1;
      if (num < screen.currentMenu->optionsLen && num >= 0) {
        screen.currentMenu->action(num, screen);
        screen.currentMenu = screen.currentMenu->nextMenus[num];
        screen.clearMenu();
        screen.display();
      } else if (key == '*') {
        if (screen.currentMenu->prevMenu != nullptr) {
          screen.currentMenu = screen.currentMenu->prevMenu;
          screen.clearMenu();
          screen.display();
        }
      }
    }
  }
};

Input input;

class Mesh {
public:
  XBee xbee;
  ZBRxResponse rxResponse;
  String MAC_ADR;
  String landADR;
  String sendToADR;

  unsigned long lastSendTime;
  unsigned long sendInterval = 500;

  Mesh() : xbee(XBee()), rxResponse(ZBRxResponse()), MAC_ADR("0013A2004218639F"), landADR("0013A2004218639F"), sendToADR(""), lastSendTime(0) {}

  void start() {
    xbeeSerial.begin(115200);
    xbee.begin(xbeeSerial);
  }

  XBeeAddress64 StringToXBeeAddress64(const String addrStr) {
      // Ensure the string is the correct length
      if (addrStr.length() != 16) {
          // Return a default address if the string is invalid
          return XBeeAddress64();
      }
      
      // Split the string into high and low parts
      String highStr = addrStr.substring(0, 8);
      String lowStr = addrStr.substring(8, 16);
      
      // Convert the strings to 32-bit integers
      uint32_t high = strtoul(highStr.c_str(), NULL, 16);
      uint32_t low = strtoul(lowStr.c_str(), NULL, 16);
      
      // Create and return an XBeeAddress64 object
      return XBeeAddress64(high, low);
  }

  void xbee_SendDataFrame(double data[], int dataLength, String addr) {
    uint8_t payload[dataLength * sizeof(double)];
    memcpy(payload, data, dataLength * sizeof(double));
    XBeeAddress64 addr64 = StringToXBeeAddress64(addr);
    ZBTxRequest txRequest = ZBTxRequest(addr64, payload, sizeof(payload));
    
    // Send the message
    xbee.send(txRequest);

    // Check the status of the transmission
    // if (xbee.readPacket(500)) {
    //   if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
    //     ZBTxStatusResponse txStatus;
    //     xbee.getResponse().getZBTxStatusResponse(txStatus);
    //     if (txStatus.getDeliveryStatus() == SUCCESS) {
          // Serial.println("Success");
    //     } else {
          // Serial.println("Failure");
    //     }
    //   }
    // } else {
      // Serial.println("Transmission timeout");
    // }
  }

  void xbee_ReceiveDataFrame() {
    xbee.readPacket();

    if (xbee.getResponse().isAvailable()) {
      // Check if we received a ZB RX packet
      if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
        xbee.getResponse().getZBRxResponse(rxResponse);

        // Get the byte data
        int byteDataLength = rxResponse.getDataLength();
        uint8_t* byteData = rxResponse.getData();

        // Convert the byte array back into a double array
        int numDoubles = byteDataLength / sizeof(double);
        double receivedData[numDoubles];
        memcpy(receivedData, byteData, byteDataLength);

        parseData(receivedData, numDoubles);
      }
    } else if (xbee.getResponse().isError()) {
      // Serial.print("Error reading packet. Error code: ");
      // Serial.println(xbee.getResponse().getErrorCode());
    }
  }

  void parseData(double receivedData[], int len) {
    int dataType = receivedData[0];
    int fromDrone = receivedData[1];
    int sourceDrone = receivedData[2];
    double* dataArray = droneList[sourceDrone].dataList[dataType];
    for (int i = 3; i < len; i++) {
      dataArray[i - 3] = receivedData[i];
    }
    droneList[sourceDrone].dataList[dataType] = dataArray;
    if (sourceDrone == currentDrone) {
      screen.updateData();
    }
    if (dataType == 1 && !droneList[sourceDrone].transferData) {
      droneList[sourceDrone].transferData = true;
    }
  }
};

Mesh mesh;

void setup() {
  Serial.begin(115200);
  mesh.start();
  screen.start();
}

void loop() {
  currentTime = millis();
  mesh.xbee_ReceiveDataFrame();
  input.checkInput();

  droneList[transferCheckIndex].checkTransferData();
  transferCheckIndex = (transferCheckIndex + 1) % 4;

  if (currentTime - droneList[currentDrone].lastSendTime > droneList[currentDrone].sendInterval) {
    analogInput.checkInputs();
    double data[3] = {-1, analogInput.rudder, analogInput.propellor};
    mesh.xbee_SendDataFrame(data, 3, droneList[currentDrone].MAC_ADR);
    droneList[currentDrone].lastSendTime = currentTime;
  }

  if (screen.readyToSend) {
    mesh.xbee_SendDataFrame(screen.command, screen.commandLength, droneList[currentDrone].MAC_ADR);
    screen.readyToSend = false;
  }
}
