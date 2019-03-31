// put your setup code here, to run once:
bool path_condition; // To make sure Bot is in it's path

// PID variables
bool stable = true, unstable = false;
float hist_error = 0;
float Kp = 0.05, Kd = 0.8, Ki = 0;
int threshold;
int last_error = 0;
int Speed, slowSpeed;
int maxspeed = 100;
int mean = 100;
int piderror, error;
  
// Just to make things easier
String left = "left"; 
String right = "right";
String forward = "forward";
String lastdir;

//Setting up pins
int Center = A2;
int Left = A0;  
int Right =A4 ;
int CenterRight = A3;
int CenterLeft = A1;
int LeftMotor = 3;
int LeftMotorRev = 9;
int RightMotor =5 ;
int RightMotorRev = 10;

// Some variables to calculate various errors

int left_reading, centerleft_reading, right_reading, centerright_reading, center_reading, left_error, right_error;
int propotional_error, derivative_error, integral_error;

void setup() {
  
  pinMode(Center, INPUT);
  pinMode(Right, INPUT);
  pinMode(Left, INPUT);
  pinMode(CenterRight, INPUT);
  pinMode(CenterLeft, INPUT);
  pinMode(LeftMotor, OUTPUT);
  pinMode(RightMotor, OUTPUT);
  Serial.begin(9600);
}


int moveBot(String dir){
  // Applying PID
  propotional_error = Kp*error;
  derivative_error = Kd*(error-last_error);
  integral_error = Ki*(error + last_error);
  
  piderror = propotional_error + integral_error + derivative_error; // Total Error
  
  if(dir == forward){
    analogWrite(LeftMotor, (maxspeed + piderror));
    analogWrite(RightMotor, (maxspeed - piderror));    
  }
  
  else if(dir == left){
    analogWrite(LeftMotorRev, maxspeed);
    analogWrite(RightMotor, maxspeed*2);
    analogWrite(LeftMotor, 0);
    analogWrite(RightMotorRev, 0);
  }
  
  else if(dir == right){
    analogWrite(LeftMotor, maxspeed*2);
    analogWrite(RightMotorRev, maxspeed);
    analogWrite(LeftMotorRev, 0);
    analogWrite(RightMotor, 0);
  }
}


void getreading(){
  /* This fuction takes in the readings from the 5 sensors attached to the bot and calculates error */
    left_reading = 1023 - analogRead(Left);
    centerleft_reading = 1023 - analogRead(CenterLeft);
    right_reading = 1023 - analogRead(Right);
    centerright_reading = 1023 - analogRead(CenterRight);
    center_reading = 1023 - analogRead(Center);
    left_error = left_reading + centerleft_reading;
    right_error = right_reading + centerright_reading;
    error = left_reading + centerleft_reading - right_reading - centerright_reading - mean;
}


void loop() {
  getreading();
  hist_error = max(hist_error, abs(error));
  if(hist_error == abs(error)){
    if(error > 0){
      lastdir = right;
    }
    else{
      lastdir = left;
    }
  }
  while(left_error < 1400){
    getreading();
    moveBot(left);
  }
  if(center_reading < 550){
    if(abs(error) < 50){
      hist_error = 0;
    }
    moveBot(forward);
  }
  else{
    if(right_error < 1400){
      while(right_error < 1400){
        getreading();
        moveBot(right);    
      }
    }
    else{
      while(min(center_reading, min(right_reading, min(left_reading, min(centerleft_reading, centerright_reading)))) > 700){
        getreading();
        if(lastdir == left){
          moveBot(left);
        }
        else{
          moveBot(right);
        }
      }
    }
  }
  last_error = error;
}
