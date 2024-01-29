#include "hook.h"
#include "logging.h"
#include <string>

using namespace std;

static jstring (*orig_native_get)(JNIEnv *env, jclass clazz, jstring keyJ, jstring defJ);

static jstring my_native_get(JNIEnv *env, jclass clazz, jstring keyJ, jstring defJ) {
    const char *key = env->GetStringUTFChars(keyJ, nullptr);
    const char *def = env->GetStringUTFChars(defJ, nullptr);

    LOGD("my_native_get(*env, clazz, %s, %s)\n", key, def);

    jstring hooked_result = nullptr;

    // MIUI
    if (strcmp(key, "ro.product.brand") == 0) { // ro.product.brand=Xiaomi
        hooked_result = env->NewStringUTF("Xiaomi");
    } else if (strcmp(key, "ro.product.manufacturer") == 0) { // ro.product.manufacturer=Xiaomi
        hooked_result = env->NewStringUTF("Xiaomi");
    } else if (strcmp(key, "ro.miui.ui.version.name") == 0) { // ro.miui.ui.version.name=V12
        hooked_result = env->NewStringUTF("V12");
    } else if (strcmp(key, "ro.miui.ui.version.code") == 0) { // ro.miui.ui.version.code=10
        hooked_result = env->NewStringUTF("10");
    } else if (strcmp(key, "ro.miui.version.code_time") == 0) { // ro.miui.version.code_time=1592409600
        hooked_result = env->NewStringUTF("1592409600");
    } else if (strcmp(key, "ro.miui.internal.storage") == 0) { // ro.miui.internal.storage=/sdcard/
        hooked_result = env->NewStringUTF("/sdcard/");
    } else if (strcmp(key, "ro.miui.region") == 0) { // ro.miui.region=CN
        hooked_result = env->NewStringUTF("CN");
    } else if (strcmp(key, "ro.miui.cust_variant") == 0) { // ro.miui.cust_variant=cn
        hooked_result = env->NewStringUTF("cn");
    } else {
        // LOGD("orig_native_get()\n");
        // hooked_result = orig_native_get(env, clazz, keyJ, defJ);
        // LOGD("orig_native_got()\n");
        hooked_result = env->NewStringUTF("?");
        LOGD("orig_native_forged()\n");
    }

    env->ReleaseStringUTFChars(keyJ, key);
    env->ReleaseStringUTFChars(defJ, def);

#ifdef DEBUG
    const char *result = env->GetStringUTFChars(hooked_result, nullptr);
    LOGD("my_native_get(): %s\n", result);
    env->ReleaseStringUTFChars(hooked_result, result);
#endif
    return hooked_result;
}

void hookBuild(JNIEnv *env) {
    LOGD("hook Build\n");
    jclass build_class = env->FindClass("android/os/Build");
    jstring new_brand = env->NewStringUTF("Xiaomi");
    jstring new_manufacturer = env->NewStringUTF("Xiaomi");

    jfieldID brand_id = env->GetStaticFieldID(build_class, "BRAND", "Ljava/lang/String;");
    if (brand_id != nullptr) {
        env->SetStaticObjectField(build_class, brand_id, new_brand);
    }

    jfieldID manufacturer_id = env->GetStaticFieldID(build_class, "MANUFACTURER", "Ljava/lang/String;");
    if (manufacturer_id != nullptr) {
        env->SetStaticObjectField(build_class, manufacturer_id, new_manufacturer);
    }

    env->DeleteLocalRef(new_brand);
    env->DeleteLocalRef(new_manufacturer);

    LOGD("hook Build done");
}

void hookSystemProperties(JNIEnv *env, zygisk::Api *api) {
    LOGD("hook SystemProperties\n");

    JNINativeMethod targetHookMethods[] = {
            {"native_get", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;",
             (void *) my_native_get},
    };

    api->hookJniNativeMethods(env, "android/os/SystemProperties", targetHookMethods, 1);

    *(void **) &orig_native_get = targetHookMethods[0].fnPtr;

    LOGD("hook SystemProperties done: %p\n", orig_native_get);
}

void Hook::hook() {
    hookBuild(env);
    hookSystemProperties(env, api);
}