#define gest 4

float thresh[4] = {0.65,0.6,0.5,0.45};
int pins[4] = {0,1,2,3};
  //**Default Ranges**//
//Gauge #1: id  = 35, pull-up R  = 400k range ~ [612, 700]
//Gauge #2: id  = 9, pull-up R  = 300k range ~ [435, 556]
//Gauge #3: id  = 41, pull-up R  = 300k range ~ [340, 500]
//Gauge #4: id  = 49, pull-up R  = 200k range ~ [450, 550]
int range[2][4] = {
  //[0][i] = lower bound for guage i, [1][i] = upper bound for guage i
  {612, 435, 340, 450},
  {700, 556, 500, 550}
};
bool flexed[4] = {0,0,0,0};
bool gestures[gest][4] = {
  {1,0,0,0},
  {0,1,0,0},
  {0,0,1,0},
  {0,0,0,1}
};
int cnt = 0;
void print_ranges();
void reset_ranges();
void update_ranges();
void start_up_script();
void listen_for_move();
int check_gesture();
bool update_flexed();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  start_up_script();
}

void loop() {
  // put your main code here, to run repeatedly:
  listen_for_move();
  
}
void start_up_script(){
  int prnt_cnt;
  bool cal, redo;
  Serial.println("Would you like to calibrate?");
  Serial.flush();
  cal = 0;
  while(!Serial.available());
  if (Serial.find("yes")){
    cal = 1;
  }
  if (cal){
    redo = 1;
    while(redo){
      Serial.println("Beginning Claibration: Enter any character to stop when satisfied");
      Serial.flush();
      reset_ranges();
      prnt_cnt = 0;
      while(!Serial.available()){
        update_ranges();
        prnt_cnt++;
        if (prnt_cnt == 5000){
          print_ranges();
          prnt_cnt = 0;
        }
      }
      while (Serial.available() && Serial.read()); // empty buffer
      Serial.println("Calibration Stopped! Final Ranges:");
      print_ranges();
      Serial.println("Are you satsfied?");
      Serial.flush();
      while (!Serial.available()); 
      if (Serial.find("yes")){
        redo = 0;
      }
      else{
        redo = 1;
      }
    }
  }
  else{
    Serial.println("Using default ranges:");
    print_ranges();
  }
  Serial.println("Waiting for actions:");
}
void print_ranges(){
  Serial.println("Ranges");
    for (int i = 0; i < 4; i++){
      Serial.print("[");Serial.print(range[0][i]);Serial.print(", "); Serial.print(range[1][i]); Serial.print("]"); Serial.print("\t");
    }
    Serial.println(" ");
}
void reset_ranges(){
  for (int i = 0; i < 4; i++){
    range[0][i] = 1023;
    range[1][i] = 0;
  }
}
void update_ranges(){
  int x;
  for (int i = 0; i < 4; i++){
    x = analogRead(pins[i]);
    if (x > range[1][i]){
      range[1][i] = x;
    }
    if (x < range[0][i]){
      range[0][i] = x;
    }
  }
}

bool update_flexed(){
  float perc;
  bool flex = 0;
  for (int i = 0; i < 4; i++){
    flexed[i] = 0;
    perc = ((float)analogRead(pins[i])-(float)range[0][i])/((float)range[1][i]-(float)range[0][i]);
    if (perc > thresh[i]){
      flexed[i] = 1;
      flex = 1;
    }
  }
  return flex;
}

void listen_for_move(){
  int gesture;
  if (update_flexed()){
    //A finger has been flexed
    gesture = check_gesture();
    if (!gesture){
      //No matching gesture
      return;
    }
    else{
      //gesture matched
      delay(100); //delay just to make sure gesture was intentional
      //Update flexed
      if (!update_flexed()){
        return;
      }
      if (check_gesture() == gesture){
        //Gesture is still the same after 100 ms -> conclude gesture was intentional
        Serial.print("Gesture: ");Serial.println(gesture);
      }
    }
  }
}

int check_gesture(){
  int r_cnt; //# matched
  for (int j = 0; j < gest; j++){
    r_cnt = 0;
    for (int i = 0; i < 4; i++){
      if (flexed[i] == gestures[j][i]){
        r_cnt++;
      }
    }
    if (r_cnt == 4){
      return (j+1);
    }
  }
  return 0;
}


