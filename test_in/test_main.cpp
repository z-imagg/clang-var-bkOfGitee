
bool calc(char op,short x, int y, float z, int& r){
  int sum=0;
  int diff;
  short mul;
  double div;

  if (op='*' )
    if (y>128)
      x=0  ;
    else if (x>64)
      y++   ;
    else
      z--   ;


  if(op='/' && y==0)
    return false;

  while(y-->0)
    mul*=x  ;


  int arg = mul/diff%23 - r;

  int arr[]={11,19,73};
  for(int j:arr)
    diff-=j*y   ;

  for(int q=0; q < q+y; q--)
    if(x+q<z)
      return true;
    else
      do{
        arg+=mul;
      }while(arg<0) ;


  while(true && sizeof (arg) > x){
    for(z--;z++;)
      while(false)
        for(y;y;)
          if(false)
            return true;
  }

  switch (x + y) {
    case 1:
      L1:break;
    case 2:
      y+z;
      goto L1;
    case 3:
      r/=x%y;
      x--;
      return false;
    case 4:
      L2:;
    case 5:
      if (x>=y)
      goto L2;
    default:
      while(true && z<y+x)
        ;
      {
        int time;
        time=x+y-z*arg;
        if(time>0)
          arg--;
        if(arg>time)
          goto L2;
      }
      return true;

  }
  return false;

}