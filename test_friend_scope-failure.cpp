struct foo
{
  friend foo f() { return {}; }
  friend foo g(foo) { return {}; }
};

int main()
{
  //ADL fails with default initializer
  g({});

// test_friend_scope.cpp:8:14: error: ‘f’ was not declared in this scope
  f();
  return 0;
}
