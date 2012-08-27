
int reverseComplement(const char *begin, const char *end, char *out){
  end -= 1;
  
  while( end != begin-1){
    
    if(*end == 'A'){
      *out = 'T';
    }else if(*end == 'T'){
      *out = 'A';
    }else if(*end == 'G'){
      *out ='C';
    }else if(*end == 'C'){
      *out ='G';
    }else if(*end == 'N'){
      *out = 'N';
    }else{
      return 0; //error
    }

    end -= 1;
    out += 1;
  }
  *out = '\0';
  return 1;
}
