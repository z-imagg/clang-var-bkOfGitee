
class User{};
class Main{
public:
  double func01();
};
int func02(double (*func_ptr) (char,int )){
  char sex;
  int age;
  func_ptr(sex, age);
}
double Main::func01(){

#ifdef  XXX
  User user;
  User & u = user;
#endif
      User self;
  return 0.0;
}