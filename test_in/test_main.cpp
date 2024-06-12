// c++ 闭包
// 模拟  jdk-jdk-24-0/src/hotspot/share/gc/g1/g1HeapVerifier.cpp
class G1HeapVerifier{
public:
  void verify_bitmap_clear(bool from_tams) ;
};

void G1HeapVerifier::verify_bitmap_clear(bool from_tams) {
  if (true) {
    return;
  }

  class G1VerifyBitmapClear   {

  public:
    G1VerifyBitmapClear( int xxx)  { }

    virtual bool do_heap_region(int* r) {
      //这里插入了destroy, 但是却没有插入init. 说明 FnVisit没认识闭包
      return false;
    }
  } cl(from_tams);

  double ddyy;
}





