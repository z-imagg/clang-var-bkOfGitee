// c++ 闭包
// 模拟  jdk-jdk-24-0/src/hotspot/share/gc/g1/g1HeapVerifier.cpp
class HeapRegionClosure{};
class G1HeapRegion{};
int heap_region_iterate(){
  return 0;
}
class G1HeapVerifier{
public:
  void verify_bitmap_clear(bool from_tams) ;
};

void G1HeapVerifier::verify_bitmap_clear(bool from_tams) {
  if (true) {
    return;
  }

  class G1VerifyBitmapClear : public HeapRegionClosure {
    bool _from_tams;

  public:
    G1VerifyBitmapClear(bool from_tams) : _from_tams(from_tams) { }

    virtual bool do_heap_region(G1HeapRegion* r) {
      //这里插入了destroy, 但是却没有插入init. 说明 FnVisit没认识闭包
      return false;
    }
  } cl(from_tams);

  heap_region_iterate();
}

int main(int argc, char** argv){

  return 0;
}




