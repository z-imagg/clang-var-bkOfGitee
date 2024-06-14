
class User{};
class Main{
public:
  double func01();
};
int func02(void (*func_ptr) (char,int )){
  char sex;
  int age;
  func_ptr(sex, age);
}
double Main::func01(){

#ifdef  XXX
  User user;
  User & u = user;
#endif
    func02([](char ch,int int_){
#ifdef  UUU
      User myuser;
  User & up = myuser;
#endif
      User self;
    });
  return 0.0;
}
