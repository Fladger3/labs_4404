#pragma once

void way1(int n);   // массив + ordered
void way2(int n);   // общий счётчик + critical
void way3(int n);   // ordered + обратный for
void way4(int n);   // массив + single + barrier
void way5(int n);   // массив + master + barrier
void way6(int n);   // atomic capture