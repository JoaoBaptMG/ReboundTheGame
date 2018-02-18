#pragma once

#define PRED(expr) ([=](auto&&... args) { return (expr); })
#define PRED_X(expr) ([=](auto&&... args, auto&& x) { return (expr); })
#define PRED_XY(expr) ([=](auto&&... args, auto&& x, auto&& y) { return (expr); })
#define PRED_XYZ(expr) ([=](auto&&... args, auto&& x, auto&& y, auto&& z) { return (expr); })