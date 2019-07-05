#include <jni.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include<vector>
#include <fstream>
#include <android/log.h>
#include <dirent.h>
#include <__hash_table>

using std::string;
using std::cout, std::cin, std::endl;

#define TAG "JNITEST"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
extern "C" JNIEXPORT jstring JNICALL
Java_com_hql_jniscan_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
void call_java_method(JNIEnv *env, jobject instance, jmethodID show_method, jstring path_) {
    env->CallVoidMethod(instance, show_method, path_);
}

void getAllFiles(string path, JNIEnv *env, jobject instance, jmethodID show_method) {
    DIR *dir;
    struct dirent *ptr;
    if ((dir = opendir(path.c_str())) == NULL) {
        LOGI("Open dri error...");
        return;
    }
    string path3;
    while ((ptr = readdir(dir)) != NULL) {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
            continue;
        else if (ptr->d_type == 8)//file
        {
            path3 = path + "/" + ptr->d_name;
            int size = strlen(ptr->d_name);
            if (ptr->d_name[0] != '.')//非隐藏文件
            {
                if ((strcmp((ptr->d_name + (size - 4)), ".mp3") == 0) ||
                    (strcmp((ptr->d_name + (size - 4)), ".MP3") == 0) ||
                    (strcmp((ptr->d_name + (size - 4)), ".WAV") == 0) ||
                    (strcmp((ptr->d_name + (size - 4)), ".wav") == 0) ||
                    (strcmp((ptr->d_name + (size - 4)), ".flac") == 0) ||
                    (strcmp((ptr->d_name + (size - 4)), ".FLAC") == 0) ||
                    (strcmp((ptr->d_name + (size - 4)), ".APE") == 0) ||
                    (strcmp((ptr->d_name + (size - 4)), ".ape") == 0)) {
                    // LOGI("符合 name > %s  " ,path3.c_str());
                    call_java_method(env, instance, show_method, env->NewStringUTF(path3.c_str()));
                }
            }

        } else if (ptr->d_type == 10)//link file
            continue;
        else if (ptr->d_type == 4) {
            getAllFiles(path + "/" + ptr->d_name, env, instance, show_method);
        }
    }
    closedir(dir);
}

std::string jstring2str(JNIEnv* env, jstring jstr)
{
    char*   rtn   =   NULL;
    jclass   clsstring   =   env->FindClass("java/lang/String");
    jstring   strencode   =   env->NewStringUTF("UTF-8");
    jmethodID   mid   =   env->GetMethodID(clsstring,   "getBytes",   "(Ljava/lang/String;)[B");
    jbyteArray   barr=   (jbyteArray)env->CallObjectMethod(jstr,mid,strencode);
    jsize   alen   =   env->GetArrayLength(barr);
    jbyte*   ba   =   env->GetByteArrayElements(barr,JNI_FALSE);
    if(alen   >   0)
    {
        rtn   =   (char*)malloc(alen+1);
        memcpy(rtn,ba,alen);
        rtn[alen]=0;
    }
    env->ReleaseByteArrayElements(barr,ba,0);
    std::string stemp(rtn);
    free(rtn);
    return   stemp;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_hql_jniscan_JNI_scanDir(JNIEnv *env, jobject instance, jstring path_) {
    std::cout << "第一个函数";
    const char *path = env->GetStringUTFChars(path_, 0);

    //std::string hello = "Hello";
    //std::string path2 = "/storage/005B-0D07";// "/storage/005B-0D07/";
    //jstring szOut = env->NewStringUTF(hello.c_str());

    jclass clazz = env->FindClass("com/hql/jniscan/JNI");
    if (clazz == NULL) {
        LOGI("find class Error");
        return;
    } else {
        LOGI("find class Success");
    }
    jmethodID show_method = env->GetMethodID(clazz, "printMediaPath", "(Ljava/lang/String;)V");
    if (show_method == NULL) {
        LOGI("find method Error");
    } else {
        LOGI("find method Success");
    }
    getAllFiles(jstring2str(env,path_), env, instance, show_method);
    env->ReleaseStringUTFChars(path_, path);
}


jboolean checkUtfBytes(const char *bytes, const char **errorKind) {
    while (*bytes != '\0') {
        jboolean utf8 = *(bytes++);
        // Switch on the high four bits.
        switch (utf8 >> 4) {
            case 0x00:
            case 0x01:
            case 0x02:
            case 0x03:
            case 0x04:
            case 0x05:
            case 0x06:
            case 0x07:
                // Bit pattern 0xxx. No need for any extra bytes.
                break;
            case 0x08:
            case 0x09:
            case 0x0a:
            case 0x0b:
            case 0x0f:
                /*
                 * Bit pattern 10xx or 1111, which are illegal start bytes.
                 * Note: 1111 is valid for normal UTF-8, but not the
                 * modified UTF-8 used here.
                 */
                *errorKind = "start";
                return utf8;
            case 0x0e:
                // Bit pattern 1110, so there are two additional bytes.
                utf8 = *(bytes++);
                if ((utf8 & 0xc0) != 0x80) {
                    *errorKind = "continuation";
                    return utf8;
                }
                // Fall through to take care of the final byte.
            case 0x0c:
            case 0x0d:
                // Bit pattern 110x, so there is one additional byte.
                utf8 = *(bytes++);
                if ((utf8 & 0xc0) != 0x80) {
                    *errorKind = "continuation";
                    return utf8;
                }
                break;
        }
    }
    return 0;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_hql_jniscan_MainActivity_encryptStr(JNIEnv *env, jobject instance, jstring data) {
    if (data == NULL) {
        return env->NewStringUTF("");
    }
    jsize len = env->GetStringLength(data);
    char *buffer = (char *) malloc(len * sizeof(char));
    env->GetStringUTFRegion(data, 0, len, buffer);
    int i = 0;
    for (; i < len; i++) {
        buffer[i] = (char) (buffer[i] ^ 2);
    }

    const char *errorKind = NULL;
    checkUtfBytes(buffer, &errorKind);
    free(buffer);
    if (errorKind == NULL) {
        return env->NewStringUTF(buffer);
    } else {
        return env->NewStringUTF("");
    }
}