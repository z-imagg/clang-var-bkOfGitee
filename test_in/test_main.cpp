class User{};

int func02(double (*func_ptr) (char,int )){
  char sex;
  int age;
  func_ptr(sex, age);
}
double func01(){
  User user;
  User & u = user;
  if(&u!= nullptr){
    func02([](char ch,int int_){
      User self;
      return 1.0;
    });
  }
  return 0.0;
}