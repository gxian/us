#pragma once
// #include <cinttypes>
// #include <iostream>

namespace us
{
  class Context
  {
  public:
    Context() {};
    ~Context() {};

  };

  void RestoreContext(const Context* c)
  {
    __asm__ (
      "movq	16(%rdi), %rsi\n\t"
      "movq	24(%rdi), %rdx\n\t"
      "movq	32(%rdi), %rcx\n\t"
      "movq	40(%rdi), %r8\n\t"
      "movq	48(%rdi), %r9\n\t"
      "movq	56(%rdi), %rax\n\t"
      "movq	64(%rdi), %rbx\n\t"
      "movq	72(%rdi), %rbp\n\t"
      "movq	80(%rdi), %r10\n\t"
      "movq	88(%rdi), %r11\n\t"
      "movq	96(%rdi), %r12\n\t"
      "movq	104(%rdi), %r13\n\t"
      "movq	112(%rdi), %r14\n\t"
      "movq	120(%rdi), %r15\n\t"
      "movq	184(%rdi), %rsp\n\t"
      "pushq	160(%rdi)\n\t"
      "movq	8(%rdi), %rdi\n\t"
      "ret\n\t"
             );
  }

    int SaveContext(Context* c)
    {
      __asm__ (
        "movq	%rdi, 8(%rdi)\n\t"
        "movq	%rsi, 16(%rdi)\n\t"
        "movq	%rdx, 24(%rdi)\n\t"
        "movq	%rcx, 32(%rdi)\n\t"
        "movq	%r8, 40(%rdi)\n\t"
        "movq	%r9, 48(%rdi)\n\t"
        "movq	$1, 56(%rdi)\n\t"
        "movq	%rbx, 64(%rdi)"
        "movq	%rbp, 72(%rdi)"
        "movq	%r10, 80(%rdi)"
        "movq	%r11, 88(%rdi)"
        "movq	%r12, 96(%rdi)"
        "movq	%r13, 104(%rdi)"
        "movq	%r14, 112(%rdi)"
        "movq	%r15, 120(%rdi)"

        "movq	(%rsp), %rcx"
        "movq	%rcx, 160(%rdi)"
        "leaq	8(%rsp), %rcx"
        "movq	%rcx, 184(%rdi)"

        "movq	32(%rdi), %rcx"
        "movq	$0, %rax"
        "ret"
               );
    }

  // template<typename ...T, typename R>
  // class Task
  // {
  // public:

  // private:
  //   std::function<R (T...)> func_;

  // };

  // class Codec
  // {
  // public:
  //   void Decode() {}
  //   void Encode() {}
  // };

  // class Server
  // {
  // public:

  // private:

  // };

  // class Client
  // {
  // public:
  // private:

  // };





}

