#define STATE_WAITING_FOR_SENTENCE_START -1
#define STATE_READING_NMEA_TAG 0
#define MSG_NONE 0
#define MSG_GGA 1
#define MSG_RMC 2
#define MSG_ZDA 3

String rxBuffer;  //incoming bytes stacked here, and cleared after each tag or field
String txBuffer;  //outgoing bytes stacked here, and opportunistically written to serial out
String ChecksumSequence; //keeping a copy of whole nmea string to checksum at end of send
char rxChar;  //the actual char read from serial1
int rxState;  //positive values indicate the field number being read
int MSGTag;
bool PositionFix, QualityGood;
String LastLat, LastLatDir, LastLong, LastLongDir;

void setup() {
  Serial1.begin(9600);
  rxBuffer.reserve(255);
  rxBuffer = "";
  rxState = STATE_WAITING_FOR_SENTENCE_START;
  txBuffer.reserve(255);
  ChecksumSequence.reserve(255);
  txBuffer = "$GPTXT,01,01,01,System Bootup*78\r\n"; //dbgmsg
  MSGTag = MSG_NONE;
  PositionFix = false;
  QualityGood = false;
  LastLat.reserve(16);
  LastLong.reserve(16);
  LastLatDir.reserve(16);
  LastLongDir.reserve(16);
  LastLat = "         ";
  LastLong = "          ";
  LastLatDir = " ";
  LastLongDir = " ";
}

void ParseTag() {
  if (rxBuffer.length() < 6){ //should be 5 characters plus trailing comma
    MSGTag = MSG_NONE;
    rxState = STATE_WAITING_FOR_SENTENCE_START;
  }
  else if (rxBuffer[0] != 'G'){ //checking first char, because we ignore second char constellation byte
    MSGTag = MSG_NONE;
    rxState = STATE_WAITING_FOR_SENTENCE_START;
  }
  else
  {
    rxBuffer.remove(0,2);
    if (rxBuffer == "GGA,"){
      MSGTag = MSG_GGA;
      if (PositionFix) {
        txBuffer += "$GPGGA,";
        ChecksumSequence = "GPGGA,";
      }
      rxState = 1;
    }
    else if (rxBuffer == "RMC,") {
      MSGTag = MSG_RMC;
      if (PositionFix) {
        txBuffer += "$GPRMC,";
        ChecksumSequence = "GPRMC,";
      }
      else
      {
        txBuffer += "$PDBG,";
        ChecksumSequence = "PDBG,";
      }
      rxState = 1;
    }
    else if (rxBuffer == "ZDA,"){
      MSGTag = MSG_ZDA;
      if (PositionFix) {
        txBuffer += "$GPZDA,";
        ChecksumSequence = "GPZDA,";
      }
      else
      {
        txBuffer += "$PDBG,";
        ChecksumSequence = "PDBG,";
      }

      rxState = 1;
    }
    else {//unknown nmea type
       MSGTag = MSG_NONE;
       rxState = STATE_WAITING_FOR_SENTENCE_START;
    }
  }
   

  rxBuffer = ""; //tag has been read, so clear the rx buffer

}

void ParseField() {
  switch (MSGTag){
    case MSG_GGA:
       ParseGGA();
       break;
    case MSG_RMC:
       ParseRMC();
       break;
    case MSG_ZDA:
       ParseZDA();
       break;
  }
  rxState++;
}

void ParseGGA() {
  rxBuffer.remove(rxBuffer.length() - 1);
  switch (rxState) {
    case 1: //Timefield 
      rxBuffer.remove(rxBuffer.length() - 1);
      if (rxBuffer.length() >0){
        txBuffer+=NumberParser(rxBuffer,6,3);
        
        ChecksumSequence += NumberParser(rxBuffer,6,3);
        
      } 
      else {
        txBuffer+="          ";
        ChecksumSequence += "          ";
      }
      
      txBuffer += ',';
      ChecksumSequence += ',';
    break;
    case 2: //Latitude
      if (rxBuffer.length() >0){
        txBuffer+=NumberParser(rxBuffer,4,4);
        ChecksumSequence += NumberParser(rxBuffer,4,4);
      }
      else {
        txBuffer+= LastLat;
        ChecksumSequence += LastLat;
      }

      txBuffer += ',';
      ChecksumSequence += ',';
    break;
    case 3: // N or S
      if (rxBuffer.length() >0){
        txBuffer+=rxBuffer[0];
        ChecksumSequence += rxBuffer[0];
      }
      else {
        txBuffer+= LastLatDir;
        ChecksumSequence += LastLatDir;
      }
      txBuffer += ',';
      ChecksumSequence += ',';
    break;
    case 4: // Longitude
      if (rxBuffer.length() >0){
        txBuffer+=NumberParser(rxBuffer,5,4);
        ChecksumSequence += NumberParser(rxBuffer,5,4);
      }
      else {
        txBuffer+= LastLong;
        ChecksumSequence += LastLong;
      }
      txBuffer += ',';
      ChecksumSequence += ',';
    break;
    case 5: // E or W
      if (rxBuffer.length() >0){
        txBuffer+=rxBuffer[0];
        ChecksumSequence += rxBuffer[0];
      }
      else {
        txBuffer+= LastLongDir;
        ChecksumSequence += LastLongDir;
      }
      txBuffer += ',';
      ChecksumSequence += ',';
    break;
    case 6: //Quality
      if (rxBuffer.length() >0){
        txBuffer+=rxBuffer[0];
        ChecksumSequence += rxBuffer[0];
        if ((rxBuffer[0] == '1') || (rxBuffer[0] == '2') || (rxBuffer[0] == '4') || (rxBuffer[0] == '5') || (rxBuffer[0] == '9')) {
          QualityGood = true;         
        }
      }
      else {
        txBuffer+="0";
        ChecksumSequence += "0";
        QualityGood = false;
      }
      txBuffer += ',';
      ChecksumSequence += ',';
    break;
    case 7: //SatCount
      if (rxBuffer.length() >0){
        txBuffer+=NumberParser(rxBuffer,2,0);
        ChecksumSequence += NumberParser(rxBuffer,2,0);
      } 
      else {
        txBuffer+= "00";
        ChecksumSequence += "00";
      }
      txBuffer += ',';
      ChecksumSequence += ',';
    break;
    case 8: //HDop
      if (rxBuffer.length() >0){
        txBuffer+=NumberParser(rxBuffer,2,1);
        ChecksumSequence += NumberParser(rxBuffer,2,1);
      }
      else {
        txBuffer+= "99.9";
        ChecksumSequence += "99.9";
      }
      txBuffer += ',';
      ChecksumSequence += ',';

    break;
    case 9: //Altitude
      if (rxBuffer.length() >0){
        txBuffer+=NumberParser(rxBuffer,5,1);
        ChecksumSequence += NumberParser(rxBuffer,5,1);
      } 
      else {
        txBuffer+= "       ";
        ChecksumSequence += "       ";
      }
      txBuffer += ',';
      ChecksumSequence += ',';
    break;
    case 10: //Alt Units
        if (rxBuffer.length() >0){
        txBuffer+=rxBuffer[0];
        ChecksumSequence += rxBuffer[0];
      }
      else {
        txBuffer+= "M";
        ChecksumSequence += "M";
      }

      txBuffer += ',';
      ChecksumSequence += ',';

    break;
    case 11: //Undulation (copied)
      txBuffer += rxBuffer;
      ChecksumSequence += rxBuffer;
      txBuffer += ',';
      ChecksumSequence += ',';
    break;
    case 12://Undulation units (copied)
      txBuffer += rxBuffer;
      ChecksumSequence += rxBuffer;
      txBuffer += ',';
      ChecksumSequence += ',';
    break;
    case 13://Age (copied)
      txBuffer += rxBuffer;
      ChecksumSequence += rxBuffer;
      txBuffer += ',';
      ChecksumSequence += ',';
    break;
    case 14://Differential Station ID (copied)
      txBuffer += rxBuffer;
      ChecksumSequence += rxBuffer;
    break;

  }
  
}
void ParseRMC() {
  rxBuffer.remove(rxBuffer.length() - 1);
  switch (rxState) {
    case 1: //Timefield 
      rxBuffer.remove(rxBuffer.length() - 1);
      if (rxBuffer.length() >0){
        txBuffer+=NumberParser(rxBuffer,6,3);
        
        ChecksumSequence += NumberParser(rxBuffer,6,3);
        
      }
      else {
        txBuffer+="          ";
        ChecksumSequence += "          ";
      }
      txBuffer += ',';
      ChecksumSequence += ',';
    break;
    case 2: //Fix validity
      if (rxBuffer.length() >0){
        txBuffer+=rxBuffer[0];
        ChecksumSequence += rxBuffer[0];
        if ((rxBuffer.length() == 1) && (rxBuffer[0] == 'A')){
          PositionFix = true;
        }
        else {
          // PositionFix = false;
        }
      }
      else {
        txBuffer += "V";
        ChecksumSequence += "V";
      }
      txBuffer += ',';
      ChecksumSequence += ',';

    break;

    case 3: //Latitude
      if (rxBuffer.length() >0){
        txBuffer+=NumberParser(rxBuffer,4,4);
        ChecksumSequence += NumberParser(rxBuffer,4,4);
      }
      else {
        txBuffer+= LastLat;
        ChecksumSequence += LastLat;
      }
      txBuffer += ',';
      ChecksumSequence += ',';
    break;
    case 4: // N or S
      if (rxBuffer.length() >0){
        txBuffer+=rxBuffer[0];
        ChecksumSequence += rxBuffer[0];
      }
      else {
        txBuffer+= LastLatDir;
        ChecksumSequence += LastLatDir;
      }
      txBuffer += ',';
      ChecksumSequence += ',';
    break;
    case 5: // Longitude
      if (rxBuffer.length() >0){
        txBuffer+=NumberParser(rxBuffer,5,4);
        ChecksumSequence += NumberParser(rxBuffer,5,4);
      }
      else {
        txBuffer+= LastLong;
        ChecksumSequence += LastLong;
      }
      txBuffer += ',';
      ChecksumSequence += ',';
    break;
    case 6: // E or W
      if (rxBuffer.length() >0){
        txBuffer+=rxBuffer[0];
        ChecksumSequence += rxBuffer[0];
      }
      else {
        txBuffer+= LastLongDir;
        ChecksumSequence += LastLongDir;
      }
      txBuffer += ',';
      ChecksumSequence += ',';
    break;
    case 7: //Speed
      if (rxBuffer.length() >0){
        txBuffer+=NumberParser(rxBuffer,4,2);
        ChecksumSequence += NumberParser(rxBuffer,4,2);
      }
      else 
      {
        txBuffer += "0000.00";
        ChecksumSequence += "0000.00";
      }
      txBuffer += ',';
      ChecksumSequence += ',';
    break;
    case 8: //Direction
      if (rxBuffer.length() >0){
        txBuffer+=NumberParser(rxBuffer,3,2);
        ChecksumSequence += NumberParser(rxBuffer,3,2);
      }
      else
      {
        txBuffer += "000.00";
        ChecksumSequence += "000.00";
      }
      txBuffer += ',';
      ChecksumSequence += ',';
    break;
    case 9: //Date
      if (rxBuffer.length() >0){
        txBuffer+=NumberParser(rxBuffer,6,0);
        ChecksumSequence += NumberParser(rxBuffer,6,0);
      }
      else
      {
        txBuffer += "      ";
        ChecksumSequence += "      ";
      }
      txBuffer += ',';
      ChecksumSequence += ',';
    break;
    case 10: //MagVar (copied)
      txBuffer += rxBuffer;
      ChecksumSequence += rxBuffer;
      txBuffer += ',';
      ChecksumSequence += ',';
    break;
    case 11: //Magvar (copied)
      txBuffer += rxBuffer;
      ChecksumSequence += rxBuffer;
      txBuffer += ',';
      ChecksumSequence += ',';
    break;
    case 12://Mode Indicator (copied)
      txBuffer += rxBuffer;
      ChecksumSequence += rxBuffer;
    break;

  }
    
}
void ParseZDA() {
  rxBuffer.remove(rxBuffer.length() - 1);
  switch (rxState) {
    case 1: //Timefield 
      rxBuffer.remove(rxBuffer.length() - 1);
      if (rxBuffer.length() >0){
        txBuffer+=NumberParser(rxBuffer,6,3);
        
        ChecksumSequence += NumberParser(rxBuffer,6,3);
        
      }
      txBuffer += ',';
      ChecksumSequence += ',';
    break;
    case 2: //Day of month
      if (rxBuffer.length() >0){
        txBuffer+=NumberParser(rxBuffer,2,0);
        ChecksumSequence += NumberParser(rxBuffer,2,0);
      }
      txBuffer += ',';
      ChecksumSequence += ',';
    break;
    case 3: // Month
      if (rxBuffer.length() >0){
        txBuffer+=NumberParser(rxBuffer,2,0);
        ChecksumSequence += NumberParser(rxBuffer,2,0);
      }
      txBuffer += ',';
    break;
    case 4: // Year
      if (rxBuffer.length() >0){
        txBuffer+=NumberParser(rxBuffer,4,0);
        ChecksumSequence += NumberParser(rxBuffer,4,0);
      }
      txBuffer += ',';
    break;
    case 5: // Local Zone (ignored)
      txBuffer += ',';
      ChecksumSequence += ',';
    break;
    case 6:// Local Zone (ignored)
    break;
  }
  
}

void DoChecksum() {
unsigned int CheckSum = 0;
String ChecksumText = "";
int i;
  ChecksumText.reserve(4);
  if (MSGTag > 0) {
      for (i=0;i<ChecksumSequence.length();i++){
        CheckSum = CheckSum xor ChecksumSequence[i];
      }
        ChecksumText = String(CheckSum, HEX);
        ChecksumText.toUpperCase();
        txBuffer += "*";
        txBuffer += ChecksumText;
        txBuffer += "\r\n"; 
      
  }

}


String SigFigs (String InputString){
  if (InputString.indexOf('.') > 0) {
    InputString.remove(InputString.indexOf('.'));
  }
  return InputString;
 
}

String DecPlaces (String InputString){
  if (InputString.indexOf('.') >= 0) {
    InputString.remove(0,InputString.indexOf('.')+1);
  } 
  else {
    InputString = "";
  }

  return InputString;
 
}

String NumberParser (String InputString, int NumSigFigs, int NumDecPlaces){
  String result,prefix,suffix,Working;

  result.reserve(64);
  prefix.reserve(32);
  suffix.reserve(32);

  prefix = "";
  suffix = "";  
  
  prefix += SigFigs(InputString);
  suffix += DecPlaces (InputString);

  while (suffix.length() < NumDecPlaces){
    suffix += '0';
  }
  while (prefix.length() < NumSigFigs){
    if (prefix[0] == '-'){
      prefix = "-" + prefix;
      prefix[1] = '0';
    } 
    else{
      prefix = "0" + prefix;
    }
  }
  while (suffix.length() > NumDecPlaces){
    suffix.remove (suffix.length() - 1);
  }
  while (prefix.length() > NumSigFigs){
    prefix.remove (0,1);
  }
 
  result = prefix;
  if (NumDecPlaces > 0){
    result += '.';
    result += suffix;      
  }
  return result;
}

void stackRXChar () {
  rxChar = Serial1.read();
  //txBuffer += rxChar;//dbgecho
  if (rxBuffer.length() < 255)
  {
    rxBuffer += rxChar; 
    switch (rxChar){
      case '$':
        rxState = STATE_READING_NMEA_TAG;
        //txBuffer += "StartTag" ; //dbg
        rxBuffer = "";
        break;
      case '*':
        ParseField();
        DoChecksum();
        rxBuffer = "";
        rxState = STATE_WAITING_FOR_SENTENCE_START;
        break;
      case ',':
        //txBuffer += "Comma" ; //dbg
        //txBuffer += String(rxState); //dbg
        if (rxState == STATE_READING_NMEA_TAG){
          ParseTag();
        }
        else if (rxState > 0) {
          ParseField();
        }
        rxBuffer = "";
        break;
    }
  }      
  else //buffer overflow, so drop the rxBuffer and wait for a new sentence
  {
    rxBuffer = "";
    txBuffer += "\r\n$GPTXT,01,01,01,Debug:RXOverflow*13\r\n"; //dbgoverflow
    rxState = STATE_WAITING_FOR_SENTENCE_START;
  }
}

void putTXChar (){ // yeah, inefficient to do this 1 char at a time, but it'll work for now
  Serial1.write(txBuffer[0]);
  txBuffer.remove(0,1);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  while (Serial1.available() > 0){
    stackRXChar();
  }
  while ((txBuffer.length() > 0) && (Serial1.availableForWrite() > 0)) {
    putTXChar();
  }

   
}
