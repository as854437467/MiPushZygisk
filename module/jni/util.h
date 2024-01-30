#ifndef MIPUSHZYGISK_UTIL_H
#define MIPUSHZYGISK_UTIL_H

#include <jni.h>

static std::string jstringToStdString(JNIEnv *env, jstring &jstr) {
    if (env == nullptr || jstr == nullptr || !jstr) {
        return "";
    }

    const char *chars = env->GetStringUTFChars(jstr, nullptr);
    if (chars && *chars) {
        std::string ret(chars);
        env->ReleaseStringUTFChars(jstr, chars);
        return ret;
    }
    return "";
}

#endif //MIPUSHZYGISK_UTIL_H
