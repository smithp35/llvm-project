#include <cstdint>
#include <cstdio>

// Example to show pointer authentication of vtable pointers
// and vtable entries.

struct S {
  virtual int f();
  virtual int g();
};

int S::f() {
  printf("S::f()\n");
  return 0;
}

int S::g() {
  printf("S::g()\n");
  return 1;
}

struct D : public S {
  virtual int f() override;
  virtual int g() override;
};

int D::f() {
  printf("D::f()\n");
  return 0;
}

int D::g() {
  printf("D::g()\n");
  return 1;
}

int call_virtual(S *s) {
  // AUT vtable data-pointer in instance of s
  // AUT vtable entry code-pointers
  // Indirectly branch to code-pointers
  return s->f() + s->g();
}

int main(void) {
  S s;
  D d;
  call_virtual(&s);
  call_virtual(&d);
  return 0;
}
