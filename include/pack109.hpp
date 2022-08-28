#include <vector>
#include <string>

#ifndef PACK109_HPP
#define PACK109_HPP

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long u64;
typedef signed char i8;
typedef signed int i32;
typedef signed long i64;
typedef float f32;
typedef double f64;
typedef std::vector<u8> vec;
typedef std::string string;

#define PACK109_TRUE  0xa0
#define PACK109_FALSE 0xa1
#define PACK109_U8    0xa2
#define PACK109_U32   0xa3
#define PACK109_U64   0xa4
#define PACK109_I8    0xa5
#define PACK109_I32   0xa6
#define PACK109_I64   0xa7
#define PACK109_F32   0xa8
#define PACK109_F64   0xa9
#define PACK109_S8    0xaa
#define PACK109_S16   0xab
#define PACK109_A8    0xac
#define PACK109_A16   0xad
#define PACK109_M8    0xae
#define PACK109_M16   0xaf
#define KEY 0x2A

struct Person {
  u8 age;
  f32 height;
  std::string name;
};
struct File{
  std::string name;
  std::vector<unsigned char> bytes;
};
struct Request{
  std::string name;
};



namespace pack109 {

  void printVec(vec &bytes);
  //File
  vec serialize(struct File item);
  struct File deserialize_file(vec bytes);

  //Request
  vec serialize(struct Request item);
  struct Request deserialize_request(vec bytes);

  //Encrypt
  vec encrypt(vec item);

  // Boolean

  vec serialize(bool item);
  bool deserialize_bool(vec bytes);

  // Integers

  vec serialize(u8 item);
  u8 deserialize_u8(vec bytes);

  vec serialize(u32 item);
  u32 deserialize_u32(vec bytes);

  vec serialize(u64 item);
  u64 deserialize_u64(vec bytes);

  vec serialize(i8 item);
  i8 deserialize_i8(vec bytes);

  vec serialize(i32 item);
  i32 deserialize_i32(vec bytes);

  vec serialize(i64 item);
  i64 deserialize_i64(vec bytes);

  // Floats

  vec serialize(f32 item);
  f32 deserialize_f32(vec bytes);

  vec serialize(f64 item);
  f64 deserialize_f64(vec bytes);

  // Strings

  vec serialize(string item);
  string deserialize_string(vec bytes);
  
  // Arrays
  vec serialize(std::vector<u8> item);
  vec serialize(std::vector<u64> item);
  vec serialize(std::vector<f64> item);
  vec serialize(std::vector<string> item);

  std::vector<u8> deserialize_vec_u8(vec bytes);
  std::vector<u64> deserialize_vec_u64(vec bytes);
  std::vector<f64> deserialize_vec_f64(vec bytes);
  std::vector<string> deserialize_vec_string(vec bytes);

  // Maps
  vec serialize(struct Person item);
  struct Person deserialize_person(vec bytes);

}

#endif


using std::begin, std::end;
#define X8  256
#define X16 65536
#define MASK4 0x000000FF
#define MASK8 0x00000000000000FF

vec slice(vec& bytes, int vbegin, int vend) {
  auto start = bytes.begin() + vbegin;
  auto end = bytes.begin() + vend + 1;
  vec result(vend - vbegin + 1);
  copy(start, end, result.begin());
  return result;
}
// ----------------------------------------
// PACK109_FILE and PACK109_REQUEST
// ----------------------------------------

  //File
  vec pack109::serialize(File item){
    vec bytes;
    bytes.push_back(PACK109_M8);
    bytes.push_back(0x01); //1 k/v pair
    vec filek = serialize(string("File"));
    bytes.insert(end(bytes), begin(filek),end(filek));
    //value is an m8
    bytes.push_back(PACK109_M8);
    bytes.push_back(0x02); // 2 k/v pairs
    //k/v 1 is "name"
    vec namek = serialize(string("name"));
    //printVec(namek);
    bytes.insert(end(bytes),begin(namek),end(namek));
    //printf("item name i: %s\n",item.name.c_str());
    vec namev = serialize(item.name);
    bytes.insert(end(bytes),begin(namev),end(namev));
    //k/v 2 is "bytes"
    vec bytesk = serialize(string("bytes"));
    //printVec(bytesk);
    bytes.insert(end(bytes),begin(bytesk),end(bytesk));
    vec bytesv = serialize(item.bytes);
    bytes.insert(end(bytes),begin(bytesv),end(bytesv));
    return bytes;
  }
  struct File pack109::deserialize_file(vec bytes){
    if(bytes.size() < 10){
      throw;
    }
    vec file_slice = slice(bytes,2,7);
    string file_string = deserialize_string(file_slice);
    if(file_string != "File"){
      throw;
    }
    int nameSpot = 17 + int(bytes[17]);
    vec nameVec = slice(bytes, 16, nameSpot);
    string file_name = deserialize_string(nameVec);
    int arrayStart = 25 + bytes[17];
    //printf("int(bytes[arrayStart+1]) is %d\n",bytes[arrayStart+1]);
    //printf("arrayStart is:%d\narrayEnd is %d\n",arrayStart,arrayEnd);
    vec arrayVec = slice(bytes, arrayStart, bytes.size());
    std::vector<unsigned char> file_bytes = deserialize_vec_u8(arrayVec);
    File deserializedFile = {file_name, file_bytes};
    return deserializedFile;
  }

  //Request
  vec pack109::serialize(Request item){
    vec bytes;
    bytes.push_back(PACK109_M8);
    bytes.push_back(0x01); //1 k/v pair
    vec filek = serialize(string("Request"));
    bytes.insert(end(bytes), begin(filek),end(filek));
    //value is an m8
    bytes.push_back(PACK109_M8);
    bytes.push_back(0x01); // 1 k/v pair
    //k/v 1 is "name"
    vec namek = serialize(string("name"));
    bytes.insert(end(bytes),begin(namek),end(namek));
    vec namev = serialize(item.name);
    bytes.insert(end(bytes),begin(namev),end(namev));
    return bytes;
  }
  struct Request pack109::deserialize_request(vec bytes){
    vec request_slice = slice(bytes,2,10);
    string request_string = deserialize_string(request_slice);
    if(request_string != "Request"){
      throw;
    }
    int nameSpot = 20 + int(bytes[20]);
    vec nameVec = slice(bytes, 19, nameSpot);
    string request_name = deserialize_string(nameVec);
    Request deserializedFile = {.name = request_name};
    return deserializedFile;
  }

// ----------------------------------------
//PACK109_ENCRYPT
// ----------------------------------------
  vec pack109::encrypt(vec item){
    vec bytes;
    for(int i = 0; i < item.size(); i++){
      //printf("item[%d] is: %d\n",i,item[i]);
      //printf("item[%d]^KEY is: %d\n",i,(u8)item[i]^KEY);
      bytes.push_back((u8)item[i]^KEY);
    }
    return bytes;
  }

/// ----------------------------------------
// PACK109_TRUE and PACK109_FALSE
// ----------------------------------------

vec pack109::serialize(bool item) {
  vec bytes;
  if (item == true) {
    bytes.push_back(PACK109_TRUE);
  } else {
    bytes.push_back(PACK109_FALSE);
  }
  return bytes;
}

bool pack109::deserialize_bool(vec bytes) {
  if (bytes.size() < 1) {
     throw;
  }

  if (bytes[0] == PACK109_TRUE) {
    return true;
  } else if (bytes[0] == PACK109_FALSE) {
    return false;
  } else {
    throw;
  }

}

// ----------------------------------------
// PACK109_U8
// ----------------------------------------

vec pack109::serialize(u8 item) {
  vec bytes;
  bytes.push_back(PACK109_U8); //push tag
  bytes.push_back(item); //push byte
  return bytes;
}

u8 pack109::deserialize_u8(vec bytes) {
  if (bytes.size() < 2) { // need two bytes for u8 -- one for tag and one for the byte
    throw;
  }
  if (bytes[0] == PACK109_U8) { // check for correct tag
    return bytes[1];
  } else {
    throw;
  }
}

// ----------------------------------------
// PACK109_U32
// ----------------------------------------

vec pack109::serialize(u32 item){
  vec bytes;
  u32 temp;
  u32 temp2;
  bytes.push_back(PACK109_U32);
  for(int i = 6; i >= 0; i-=2){
    //shift in byte size intervals, starting from the beggining of number
    temp = item >> 4 * i;
    temp2 = temp & 0x000000FF;
    //push casted byte to the back of the vector
    bytes.push_back((u8)temp2);
  }
  return bytes;
}

u32 pack109::deserialize_u32(vec bytes){
  if(bytes.size() != 5){ // need tag and then 4 bytes
    throw;
  }
  u32 item = 0x00000000; // container for result
  if(bytes[0] == PACK109_U32){ // check for correct tag
    for(int i = 1; i < 5; i++){ //take byte at a time and shift into correct position
      item = item | (u32)bytes[i] << (6+2*(1-i))*4;
    }
    return item;
  }
}

// ----------------------------------------
// PACK109_U64
// ----------------------------------------
//these functions are almost the same as the u32, but just has some adjustments for the sizing
vec pack109::serialize(u64 item){
  vec bytes;
  u64 temp;
  u64 temp2;
  bytes.push_back(PACK109_U64);
  for(int i = 14; i >= 0; i-=2){
    temp = item >> 4 * i;
    temp2 = temp & 0x00000000000000FF;
    bytes.push_back((u8)temp2);
  }
  return bytes;
}
u64 pack109::deserialize_u64(vec bytes){
  if(bytes.size() != 9){
    throw;
  }
  u64 item = 0x0000000000000000;
  if(bytes[0] == PACK109_U64){
    for(int i = 1; i < 9; i++){
      item = item | (u64)bytes[i] << (14+2*(1-i))*4;
    }
    return item;
  }
}

// ----------------------------------------
// PACK109_I8
// ----------------------------------------

vec pack109::serialize(i8 item){
  vec bytes;
  bytes.push_back(PACK109_I8);
  bytes.push_back((u8)item);
  return bytes;
}
i8 pack109::deserialize_i8(vec bytes){
  if (bytes.size() < 2) {
    throw;
  }
  if (bytes[0] == PACK109_I8) {
    return (i8)bytes[1];
  } else {
    throw;
  }
}

// ----------------------------------------
// PACK109_I32
// ----------------------------------------

vec pack109::serialize(i32 item){
  vec bytes;
  i32 temp;
  i32 temp2;
  bytes.push_back(PACK109_I32);
  //printf("item is %x\n",item);
  for(int i = 6; i >= 0; i-=2){
    temp = item >> 4 * i;
    //printf("temp for int %d is: %x\n",i,temp);
    temp2 = temp & 0x000000FF;
    //printf("temp2 for int %d is: %x\n",i,temp2);
    bytes.push_back((u8)temp2);
  }
  return bytes;
}
i32 pack109::deserialize_i32(vec bytes){
  if(bytes.size() != 5){
    throw;
  }
  i32 item = 0x00000000;
  if(bytes[0] == PACK109_I32){
    for(int i = 1; i < 5; i++){
      item = item | (i32)bytes[i] << (6+2*(1-i))*4;
    }
    return item;
  }
}

// ----------------------------------------
// PACK109_I64
// ----------------------------------------

vec pack109::serialize(i64 item){
  vec bytes;
  i64 temp;
  i64 temp2;
  bytes.push_back(PACK109_I64);
  for(int i = 14; i >= 0; i-=2){
    temp = item >> 4 * i;
    temp2 = temp & 0x00000000000000FF;
    bytes.push_back((u8)temp2);
  }
  return bytes;
}
i64 pack109::deserialize_i64(vec bytes){
  if(bytes.size() != 9){
    throw;
  }
  i64 item = 0x0000000000000000;
  if(bytes[0] == PACK109_I64){
    for(int i = 1; i < 9; i++){
      item = item | (i64)bytes[i] << (14+2*(1-i))*4;
    }
    return item;
  }
}

// ----------------------------------------
// PACK109_F32
// ----------------------------------------

vec pack109::serialize(f32 item){
  
  vec bytes;
  //printf("item is:%d\n",item);
  u32 val = *(u32*)&item;
  // *(u32*)(&item)
  //printf("val is: %d\n",val);
  u32 temp;
  u32 temp2;
  //printf("item is %x\n",val);
  bytes.push_back(PACK109_F32);
  for(int i = 6; i >= 0; i-=2){
    temp = val >> 4 * i;
    //printf("temp for int %d is: %x\n",i,temp);
    temp2 = temp & 0x000000FF;
    //printf("temp2 for int %d is: %x\n",i,temp2);
    bytes.push_back(temp2);
  }
  return bytes;
}
f32 pack109::deserialize_f32(vec bytes){
  if(bytes.size() != 5){
    throw;
  }
  u32 item = 0x00000000;
  if(bytes[0] == PACK109_F32){
    int j = 6;
    for(int i = 1; i < 5; i++){
      item = item | (u32)bytes[i] << j*4;
      //printf("bytes[%d] is %x\n",i,bytes[i]);
      //printf("tem is now %x\n",item);
      j-=2;
    }
  }
  return *(f32*)&item;
}

// ----------------------------------------
// PACK109_F64
// ----------------------------------------

vec pack109::serialize(f64 item){
  vec bytes;
  u64 val = *(u64*)&item;
  u64 temp;
  u64 temp2;
  bytes.push_back(PACK109_F64);
  for(int i = 14; i >= 0; i-=2){
    temp = val >> 4 * i;
    temp2 = temp & 0x00000000000000FF;
    bytes.push_back(temp2);
  }
  return bytes;
}
f64 pack109::deserialize_f64(vec bytes){
  if(bytes.size() != 9){
    throw;
  }
  u64 item = 0x0000000000000000;
  if(bytes[0] == PACK109_F64){
    int j = 14;
    for(int i = 1; i < 9; i++){
      item = item | (u64)bytes[i] << j*4;
      //printf("bytes[%d] is %lx\n",i,bytes[i]);
      //printf("tem is now %lx\n",item);
      j-=2;
    }
  }
  return *(f64*)&item;
}

// ----------------------------------------
// PACK109_S8 and PACK109_S16
// ----------------------------------------
vec pack109::serialize(string item){
  vec bytes;
  if(item.size() <= 255){ //can handle S8 and S16 in same function so we just check the size
    bytes.push_back(PACK109_S8); //push tag
    u8 stringSize = item.size();
    bytes.push_back((u8)stringSize); //push size of string
  }
  else if(item.size() < 65536){
    bytes.push_back(PACK109_S16); //push tag
    u32 size = item.size();
    //split size of string into two bytes and push the two
    bytes.push_back((u8)size >> 8 & 0x000000FF);
    bytes.push_back((u8)size & 0x000000FF);
  }
  else{
    throw;
  }
  for(int i = 0; i < item.size() ; i++){
    bytes.push_back(item[i]); //push each byte of string into vector
  }
  return bytes;
}
string pack109::deserialize_string(vec bytes){
  int i;
  int size;
  if(bytes[0] == PACK109_S8){ //check size since we handle S8 and S16 in the same function
    i = 2;
    size = bytes[1];
  }
  else if(bytes[0] == PACK109_S16){
    i = 3;
    size = 0x00000000;
    for(int z = 1; z < 3; z++){ //put string size back together from two bytes in the vector
      size = size | bytes[z] << (2-z)*8;
    }
  }
  else{
    throw;
  }
  string returnVal = "";
  int f = i;
  for(i; i < size+f; i++){ //loop through and reconstruct the string starting from the appropriate position
    //printf("bytes[%d] is %x\n",i,bytes[i]);
    returnVal.push_back(bytes[i]);
  }
  //std::cout << returnVal << "\n";
  return returnVal;
}

// ----------------------------------------
// PACK109_A8 and PACK109_A16
// ----------------------------------------

vec pack109::serialize(std::vector<u8> item){
  vec bytes;
  //check size and assign correct tag
  if(item.size() < 256){
    bytes.push_back(PACK109_A8);
    bytes.push_back((u8)item.size());
  }
  else if(item.size() < 65536){
    bytes.push_back(PACK109_A16);
    u32 size = (u32)item.size();
    //split size into two bytes and push
    bytes.push_back((u8)(size >> 8));
    bytes.push_back((u8)(size));
    printf("bytes[0] is %d and bytes[1] is %d and bytes[2] is %d\n",bytes[0],bytes[1],bytes[2]);
  }
  else{
    throw;
  }
  
  for(int i = 0; i < item.size(); i++){ //loop and push each byte since they're u8's
    bytes.push_back(PACK109_U8);
    bytes.push_back(item.at(i));
  }
  return bytes;
}
vec pack109::serialize(std::vector<u64> item){
  vec bytes;
  if(item.size() < 256){
    bytes.push_back(PACK109_A8);
    bytes.push_back((u8)item.size());
  }
  else if(item.size() < 65536){
    bytes.push_back(PACK109_A16);
    u32 size = item.size();
    //split size into two bytes and push
    bytes.push_back((u8)size >> 8 & 0x000000FF);
    bytes.push_back((u8)size & 0x000000FF);
  }
  else{
    throw;
  }
  u64 temp;
  u64 temp2;
  for(int i = 0; i < item.size(); i++){ //loop to push each u64 once serialized
    bytes.push_back(PACK109_U64);
    u64 temp3 = item.at(i);
    for(int j = 14; j >= 0; j-=2){ //for each u64, we must do the same serialization procedure
      temp = temp3 >> 4 * j;
      temp2 = temp & 0x00000000000000FF;
      bytes.push_back(temp2);
    }
  }
  return bytes;
}
vec pack109::serialize(std::vector<f64> item){
  vec bytes;
  if(item.size() < 256){
    bytes.push_back(PACK109_A8);
    bytes.push_back((u8)item.size());
  }
  else if(item.size() < 65536){
    bytes.push_back(PACK109_A16);
    u32 size = item.size();
    bytes.push_back((u8)size >> 8 & 0x000000FF);
    bytes.push_back((u8)size & 0x000000FF);
  }
  else{
    throw;
  }
  u64 temp;
  u64 temp2;
  for(int j = 0; j < item.size(); j++){
    u64 val = (u64)item.at(j);
    bytes.push_back(PACK109_F64);
    for(int i = 14; i >= 0; i-=2){
      temp = val >> 4 * i;
      temp2 = temp & 0x00000000000000FF;
      bytes.push_back(temp2);
    }
  }
  return bytes;
}
vec pack109::serialize(std::vector<string> item){
  vec bytes;
  if(item.size() < 256){
    bytes.push_back(PACK109_A8);
    bytes.push_back((u8)item.size());
  }
  else if(item.size() < 65536){
    bytes.push_back(PACK109_A16);
    u32 size = item.size();
    bytes.push_back((u8)size >> 8 & 0x000000FF);
    bytes.push_back((u8)size & 0x000000FF);
  }
  else{
    throw;
  }
  string temp;
  for(int j = 0; j < item.size(); j++){
    temp = item.at(j);
    if(temp.size() <= 255){
      bytes.push_back(PACK109_S8);
      bytes.push_back((u8)temp.size());
      //printf("temp size is:%d\n",temp.size());
      //printf("bytes[%d] is %d\n",j,bytes[j]);
    }
    else if(item.size() < 65536){
      bytes.push_back(PACK109_S16);
      u32 size = temp.size();
      bytes.push_back((u8)size >> 8 & 0x000000FF);
      bytes.push_back((u8)size & 0x000000FF);
    }
    else{
      throw;
    }
    //printf("entering string push loop\n");
    for(int i = 0; i < temp.size() ; i++){
      bytes.push_back(temp[i]);
    }
  }

  return bytes;
}

std::vector<u8> pack109::deserialize_vec_u8(vec bytes){
  int i;
  int size;
  if(bytes[0] == PACK109_A8){
    i = 2;
    size = bytes[1];
  }
  else if(bytes[0] == PACK109_A16){
    i = 3;
    size = 0x00000000;
    int j = 2;
    for(int z = 1; z < 3; z++){
      size = size | bytes[z] << j*4;
      j-=2;
    }
  }
  else{
    throw;
  }
  std::vector<u8> returnVal;
  i++;
  int f = i;
  for(i; i < 2*size+f; i += 2){
    returnVal.push_back(bytes[i]);
  }
  return returnVal;
}
std::vector<u64> pack109::deserialize_vec_u64(vec bytes){
  int i;
  int size;
  if(bytes[0] == PACK109_A8){
    i = 2;
    size = bytes[1];
  }
  else if(bytes[0] == PACK109_A16){
    i = 3;
    size = 0x00000000;
    int j = 2;
    for(int z = 1; z < 3; z++){
      size = size | bytes[z] << j*4;
      j-=2;
    }
  }
  else{
    throw;
  }
  std::vector<u64> returnVal;
  int f = i;
  u64 temp = 0x0000000000000000;
  int z = 14;
  for(i; i < 8*size+f+size; i++){
    if(bytes[i] != PACK109_U64){
      temp = temp | (u64)bytes[i] << z*4;
      //printf("(u64)bytes[i] << z*4 for i=%d z=%d is %lx\n",i,z,(u64)bytes[i] << z*4);
      z-=2;
    }
    else if(bytes[i] == PACK109_U64 && i>3){
      returnVal.push_back(temp);
      temp = 0x0000000000000000;
      z = 14;
    }
  }
  returnVal.push_back(temp);
  return returnVal;
}
std::vector<f64> pack109::deserialize_vec_f64(vec bytes){
  int i;
  int size;
  if(bytes[0] == PACK109_A8){
    i = 2;
    size = bytes[1];
  }
  else if(bytes[0] == PACK109_A16){
    i = 3;
    size = 0x00000000;
    int j = 2;
    for(int z = 1; z < 3; z++){
      size = size | bytes[z] << j*4;
      j-=2;
    }
  }
  else{
    throw;
  }
  std::vector<f64> returnVal;
  u64 temp = 0x0000000000000000;
  int z = 14;
  int f = i;
  for(i; i < 8*size+f+size; i++){
    if(bytes[i] != PACK109_F64){
      temp = temp | (u64)bytes[i] << z*4;
      z-=2;
    }
    else if(bytes[i] == PACK109_F64 && i>2){
      returnVal.push_back(f64(temp));
      temp = 0x0000000000000000;
      z = 14;
    }
  }
  returnVal.push_back(temp);
  return returnVal;
}
std::vector<string> pack109::deserialize_vec_string(vec bytes){
  int i;
  int size;
  if(bytes[0] == PACK109_A8){
    i = 2;
    size = bytes[1];
  }
  else if(bytes[0] == PACK109_A16){
    i = 3;
    size = 0x00000000;
    int j = 2;
    for(int z = 1; z < 3; z++){
      size = size | bytes[z] << j*4;
      j-=2;
    }
  }
  else{
    throw;
  }
  std::vector<string> returnVal;
  int f = i;
  string temp;
  while(i < bytes.size()){
    if(bytes[i] == PACK109_S8 || bytes[i] == PACK109_S16){
      if(i > 3){
        returnVal.push_back(temp);
        temp.clear();
      }
    }
    else{
      //printf("bytes[%d] is %c\n",i,bytes[i]);
      temp.push_back(bytes[i]);
    }
    i++;
  }
  returnVal.push_back(temp);
  return returnVal;
}


void pack109::printVec(vec &bytes) {
  printf("[ ");
  for (int i = 0; i < bytes.size(); i++) {
    printf("%d ", bytes[i]);
  }
  printf("]\n");
}