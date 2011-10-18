#ifndef IV_LV5_AERO_FLAGS_H_
#define IV_LV5_AERO_FLAGS_H_
namespace iv {
namespace lv5 {
namespace aero {

enum RegExpFlags {
  NONE = 0,
  IGNORE_CASE = 1,
  MULTILINE = 2
};

enum RegExpResult {
  AERO_FAILURE = 0,
  AERO_SUCCESS = 1,
  AERO_ERROR = 2
};


} } }  // namespace iv::lv5::aero
#endif  // IV_LV5_AERO_FLAGS_H_
