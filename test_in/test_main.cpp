class T_User {};
int main(int argc, char** argv){
  T_User zhangsan;
  int s;
  while(T_User * ptr=&zhangsan){
      s++;
  }
  return 0;
}