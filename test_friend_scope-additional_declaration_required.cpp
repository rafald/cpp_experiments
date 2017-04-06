struct foo
{
  friend foo f() { return {}; }
  friend foo g(foo) { return {}; }

  operator int() { return 0; }
};

foo f(); //REQUIRED !

int main()
{
  //ADL does not require additional declaration
  g(foo{});
// test_friend_scope.cpp:8:14: error: ‘f’ was not declared in this scope
  auto x = f();
  return x;
}
