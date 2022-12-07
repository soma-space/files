#if !defined(AFX_MY_MACRO_H_INCLUDED_)
#define AFX_MY_MACRO_H_INCLUDED_

#define RELEASE_SAFE(exp) if(exp!=NULL) { if(exp->Release() == 0) exp=NULL; }
#define DELETE_SAFE(exp) if(exp) { delete exp; exp = NULL; }
#define DELETE_ARRAY_SAFE(exp) if(exp) { delete [] exp; exp = NULL; }
#define MAKE_ZERO(exp) memset(&(exp), 0, sizeof(exp))
#define _IsKeyDown(exp) (GetAsyncKeyState(exp) & 0xff00)
#define _IsKeyDowned(exp) (GetAsyncKeyState(exp) & 0x00ff)
#define GET_ARRAY_VALUE(pointer, pitch, x, y) *((pointer) + (y)*(pitch) + (x))
#define SET_ARRAY_VALUE(pointer, pitch, x, y, val) *((pointer) + (y)*(pitch) + (x)) = val

#endif // AFX_MY_MACRO_H_INCLUDED_