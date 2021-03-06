#include <stdio.h>
#include <jni.h>
#include <cups/cups.h>
#include <cups/ppd.h>
#include <android/log.h>
#include "cups_util.h"

#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,"JNIEnv",__VA_ARGS__)

JNIEXPORT jobject JNICALL Java_com_android_printclient_fragment_fragment_SubMainFragment_getPrinters(
        JNIEnv *env, jobject jthis) {

    jclass printer;             /*a printer object class   */
    jclass client_printer_list; /*result printer list      */
    jmethodID init_printer;     /*printer class constructor*/
    jmethodID init_printer_list;/*printer list constructor */

    /*for java object Printer*/
    printer = (*env)->FindClass(env, "com/android/printclient/objects/Printer");
    if (printer == NULL)
        return NULL;
    init_printer = (*env)->GetMethodID(env, printer, "<init>", "()V");
    if (init_printer == NULL)
        return NULL;
    jfieldID printer_name = (*env)->GetFieldID(env, printer, "name", "Ljava/lang/String;");
    jfieldID printer_instance = (*env)->GetFieldID(env, printer, "instance", "Ljava/lang/String;");
    jfieldID printer_default = (*env)->GetFieldID(env, printer, "isdefault", "Z");
    jfieldID printer_uri = (*env)->GetFieldID(env, printer, "deviceuri", "Ljava/lang/String;");

    /*for return printer list*/
    client_printer_list = (*env)->FindClass(env, "java/util/ArrayList");
    if (client_printer_list == NULL)
        return NULL;
    init_printer_list = (*env)->GetMethodID(env, client_printer_list, "<init>",
                                            "()V");
    if (init_printer_list == NULL)
        return NULL;
    jobject client_printer_list_instance = (*env)->NewObject(env, client_printer_list,
                                                             init_printer_list, "");
    jmethodID client_printer_list_add = (*env)->GetMethodID(env, client_printer_list, "add",
                                                            "(Ljava/lang/Object;)Z");

    int i;                      /*for loop     */
    cups_dest_t *dests, *dest;  /*printers     */
    int dest_nums;              /*printer nums */

    dest_nums = cupsGetDests(&dests);

    for (i = dest_nums, dest = dests; i > 0; i--, dest++) {
        //new a printer object
        jobject dest_print = (*env)->NewObject(env, printer, init_printer, "");

        //convert char* to jstring as parameter
        jstring dest_name = (*env)->NewStringUTF(env, dest->name ? dest->name : "");
        jstring dest_instance = (*env)->NewStringUTF(env, dest->instance ? dest->instance : "");
        jboolean dest_default = (jboolean) dest->is_default;
        cups_option_t *temp = dest->options;
        const char *dest_device_uri = cupsGetOption("device-uri", dest->num_options, dest->options);
        jstring dest_uri = (*env)->NewStringUTF(env, dest_device_uri);

        //set printer var
        (*env)->SetObjectField(env, dest_print, printer_name, dest_name);
        (*env)->SetObjectField(env, dest_print, printer_instance, dest_instance);
        (*env)->SetBooleanField(env, dest_print, printer_default, dest_default);
        (*env)->SetObjectField(env, dest_print, printer_uri, dest_uri);

        //add printer to list
        (*env)->CallBooleanMethod(env, client_printer_list_instance, client_printer_list_add,
                                  dest_print);
    }
    return client_printer_list_instance;
}

JNIEXPORT jobject JNICALL Java_com_android_printclient_fragment_fragment_SubMainFragment_getAttributePrinter(
        JNIEnv *env, jobject jthis, jstring name, jstring instance) {

    int i; /*for loop       */
    cups_dest_t *dests; /*printers       */
    cups_dest_t *dest; /*result printer */
    int dest_num; /*printer nums   */
    jclass map; /*return map     */
    jmethodID init_map; /*constructor map*/

    dest_num = cupsGetDests(&dests);
    const char *dest_name = (*env)->GetStringUTFChars(env, name, 0);

    char *dest_instance;
    if (instance == NULL)
        dest_instance = NULL;
    else
        dest_instance = (char *) (*env)->GetStringUTFChars(env, instance, 0);

    dest = cupsGetDest(dest_name, dest_instance, dest_num, dests);
    if (dest == NULL)
        return NULL;
    cups_option_t *temp = dest->options;
    int count = dest->num_options;

    //map
    map = (*env)->FindClass(env, "java/util/HashMap");
    if (map == NULL)
        return NULL;
    init_map = (*env)->GetMethodID(env, map, "<init>", "()V");
    if (init_map == NULL)
        return NULL;
    jobject client_map_instance = (*env)->NewObject(env, map, init_map, "");
    jmethodID client_map_put = (*env)->GetMethodID(env, map, "put",
                                                   "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

    for (i = 0; i < count; i++) {
        jstring itemname = (*env)->NewStringUTF(env,
                                                temp->name ? temp->name : "");
        jstring itemvalue = (*env)->NewStringUTF(env,
                                                 temp->value ? temp->value : "");
        (*env)->CallObjectMethod(env, client_map_instance, client_map_put,
                                 itemname, itemvalue);
        temp++;
    }
    return client_map_instance;
}


JNIEXPORT jobject JNICALL Java_com_android_printclient_fragment_fragment_SubMainFragment_getJobs(
        JNIEnv *env, jobject jthis) {

    jclass job;             /*a printer object class   */
    jclass job_list; /*result printer list      */
    jmethodID init_job;     /*printer class constructor*/
    jmethodID init_job_list;/*printer list constructor */

    /*for java object Printer*/
    job = (*env)->FindClass(env, "com/android/printclient/objects/Job");
    if (job == NULL)
        return NULL;
    init_job = (*env)->GetMethodID(env, job, "<init>", "()V");
    if (init_job == NULL)
        return NULL;

    jfieldID job_id = (*env)->GetFieldID(env, job, "id", "I");
    jfieldID job_dest = (*env)->GetFieldID(env, job, "dest", "Ljava/lang/String;");
    jfieldID job_title = (*env)->GetFieldID(env, job, "title", "Ljava/lang/String;");
    jfieldID job_user = (*env)->GetFieldID(env, job, "user", "Ljava/lang/String;");
    jfieldID job_format = (*env)->GetFieldID(env, job, "format", "Ljava/lang/String;");
    jfieldID job_state = (*env)->GetFieldID(env, job, "state", "I");
    jfieldID job_priority = (*env)->GetFieldID(env, job, "priority", "I");
    jfieldID job_completed_time = (*env)->GetFieldID(env, job, "completed_time", "J");
    jfieldID job_creation_time = (*env)->GetFieldID(env, job, "creation_time", "J");
    jfieldID job_processing_time = (*env)->GetFieldID(env, job, "processing_time", "J");



    /*for return printer list*/
    job_list = (*env)->FindClass(env, "java/util/ArrayList");
    if (job_list == NULL)
        return NULL;
    init_job_list = (*env)->GetMethodID(env, job_list, "<init>",
                                        "()V");
    if (init_job_list == NULL)
        return NULL;
    jobject job_list_instance = (*env)->NewObject(env, job_list,
                                                  init_job_list, "");
    jmethodID job_list_add = (*env)->GetMethodID(env, job_list, "add",
                                                 "(Ljava/lang/Object;)Z");

    int i;                      /*for loop     */
    cups_job_t *jobs;  /*printers     */
    int job_nums;              /*printer nums */

    job_nums = cupsGetJobs(&jobs, NULL, 0, 0);

    for (i = 0; i < job_nums; i++) {
        //new a printer object
        jobject dest_job = (*env)->NewObject(env, job, init_job, "");

        //convert char* to jstring as parameter
        jstring dest_dest = (*env)->NewStringUTF(env, jobs[i].dest ? jobs[i].dest : "");
        jstring dest_title = (*env)->NewStringUTF(env, jobs[i].title ? jobs[i].title : "");
        jstring dest_user = (*env)->NewStringUTF(env, jobs[i].user ? jobs[i].user : "");
        jstring dest_format = (*env)->NewStringUTF(env, jobs[i].format ? jobs[i].format : "");
        jint dest_id = jobs[i].id;
        jint dest_state = jobs[i].state - 3;
        jint dest_priority = jobs[i].priority;
        jlong dest_completed_time = jobs[i].completed_time;
        jlong dest_creation_time = jobs[i].creation_time;
        jlong dest_processing_time = jobs[i].processing_time;


        //set printer var
        (*env)->SetObjectField(env, dest_job, job_dest, dest_dest);
        (*env)->SetObjectField(env, dest_job, job_title, dest_title);
        (*env)->SetObjectField(env, dest_job, job_user, dest_user);
        (*env)->SetObjectField(env, dest_job, job_format, dest_format);
        (*env)->SetIntField(env, dest_job, job_id, dest_id);
        (*env)->SetIntField(env, dest_job, job_state, dest_state);
        (*env)->SetIntField(env, dest_job, job_priority, dest_priority);
        (*env)->SetLongField(env, dest_job, job_completed_time, dest_completed_time);
        (*env)->SetLongField(env, dest_job, job_creation_time, dest_creation_time);
        (*env)->SetLongField(env, dest_job, job_processing_time, dest_processing_time);

        //add printer to list
        (*env)->CallBooleanMethod(env, job_list_instance, job_list_add, dest_job);
    }

    job_nums = cupsGetJobs(&jobs, NULL, 0, 1);

    for (i = 0; i < job_nums; i++) {
        //new a printer object
        jobject dest_job = (*env)->NewObject(env, job, init_job, "");

        //convert char* to jstring as parameter
        jstring dest_dest = (*env)->NewStringUTF(env, jobs[i].dest ? jobs[i].dest : "");
        jstring dest_title = (*env)->NewStringUTF(env, jobs[i].title ? jobs[i].title : "");
        jstring dest_user = (*env)->NewStringUTF(env, jobs[i].user ? jobs[i].user : "");
        jstring dest_format = (*env)->NewStringUTF(env, jobs[i].format ? jobs[i].format : "");
        jint dest_id = jobs[i].id;
        jint dest_state = jobs[i].state - 3;
        jint dest_priority = jobs[i].priority;
        jlong dest_completed_time = jobs[i].completed_time;
        jlong dest_creation_time = jobs[i].creation_time;
        jlong dest_processing_time = jobs[i].processing_time;


        //set printer var
        (*env)->SetObjectField(env, dest_job, job_dest, dest_dest);
        (*env)->SetObjectField(env, dest_job, job_title, dest_title);
        (*env)->SetObjectField(env, dest_job, job_user, dest_user);
        (*env)->SetObjectField(env, dest_job, job_format, dest_format);
        (*env)->SetIntField(env, dest_job, job_id, dest_id);
        (*env)->SetIntField(env, dest_job, job_state, dest_state);
        (*env)->SetIntField(env, dest_job, job_priority, dest_priority);
        (*env)->SetLongField(env, dest_job, job_completed_time, dest_completed_time);
        (*env)->SetLongField(env, dest_job, job_creation_time, dest_creation_time);
        (*env)->SetLongField(env, dest_job, job_processing_time, dest_processing_time);

        //add printer to list
        (*env)->CallBooleanMethod(env, job_list_instance, job_list_add, dest_job);
    }
    return job_list_instance;
}


JNIEXPORT jboolean JNICALL Java_com_android_printclient_fragment_fragment_SubMainFragment_cancelJob(
        JNIEnv *env, jobject jthis, jstring dest, jint id) {
    const char *destination = (*env)->GetStringUTFChars(env, dest, 0);
    int result = cupsCancelJob(destination, id);
    return (jboolean) result;
}


JNIEXPORT jobject JNICALL Java_com_android_printclient_PrintActivity_getPrinters(
        JNIEnv *env, jobject jthis) {
    return Java_com_android_printclient_fragment_fragment_SubMainFragment_getPrinters(env, jthis);
}

char *getServerPpd(const char *name) {
    setenv("TMPDIR", "/data/data/com.android.printclient/files", 1);
    http_t *http_t = gethttp_t();
    char *ppdfile = (char *) cupsGetPPD2(http_t, name);
    return ppdfile;
}

static ppd_file_t *ppd;

JNIEXPORT jboolean JNICALL Java_com_android_printclient_PrintActivity_init(
        JNIEnv *env, jobject jthis, jstring name) {

    char *printer_name;
    printer_name = (char *) (*env)->GetStringUTFChars(env, name, 0);
    LOGD("printer name %s", printer_name);

    setenv("TMPDIR", "/data/data/com.android.printclient/files", 1);
    char *ppdfile = getServerPpd(printer_name);

    ppd =  ppdOpenFile(ppdfile);
    if (ppd == NULL)
        return 0;

    ppdMarkDefaults(ppd);
    return 1;
}

JNIEXPORT jobject JNICALL Java_com_android_printclient_PrintActivity_getSupportPageSize(
        JNIEnv *env, jobject jthis) {

    //init object of paper
    jclass paper = (*env)->FindClass(env, "com/android/printclient/objects/Paper");
    jmethodID paper_init = (*env)->GetMethodID(env, paper, "<init>", "()V");
    jfieldID obj_name = (*env)->GetFieldID(env, paper, "name", "Ljava/lang/String;");
    jfieldID obj_width = (*env)->GetFieldID(env, paper, "width", "F");
    jfieldID obj_length = (*env)->GetFieldID(env, paper, "length", "F");
    jfieldID obj_left = (*env)->GetFieldID(env, paper, "left", "F");
    jfieldID obj_bottom = (*env)->GetFieldID(env, paper, "bottom", "F");
    jfieldID obj_right = (*env)->GetFieldID(env, paper, "right", "F");
    jfieldID obj_top = (*env)->GetFieldID(env, paper, "top", "F");
    jfieldID obj_marked = (*env)->GetFieldID(env, paper, "marked", "Z");

    //init return list of papers
    jclass papers = (*env)->FindClass(env, "java/util/ArrayList");
    jmethodID papers_init = (*env)->GetMethodID(env, papers, "<init>", "()V");
    jobject papers_instance = (*env)->NewObject(env, papers, papers_init, "");
    jmethodID papers_add = (*env)->GetMethodID(env, papers, "add", "(Ljava/lang/Object;)Z");

    int num;
    int i;
    ppd_size_t *size;

    num = ppd->num_sizes;
    size = ppd->sizes;

    for (i = 0; i < num; ++i, size++) {
        jobject *object = (*env)->NewObject(env, paper, paper_init, "");
        jstring paper_name = (*env)->NewStringUTF(env, size->name);
        (*env)->SetBooleanField(env, object, obj_marked, (jboolean) size->marked);
        (*env)->SetObjectField(env, object, obj_name, paper_name);
        (*env)->SetFloatField(env, object, obj_top, size->top);
        (*env)->SetFloatField(env, object, obj_bottom, size->bottom);
        (*env)->SetFloatField(env, object, obj_left, size->left);
        (*env)->SetFloatField(env, object, obj_right, size->right);
        (*env)->SetFloatField(env, object, obj_length, size->length);
        (*env)->SetFloatField(env, object, obj_width, size->width);

        (*env)->CallBooleanMethod(env, papers_instance, papers_add, object);
    }
    return papers_instance;
}


JNIEXPORT jobject JNICALL Java_com_android_printclient_PrintActivity_getSupportDuplex(
        JNIEnv *env, jobject jthis) {

    ppd_option_t *option = ppdFindOption(ppd, "Duplex");
    if (option == NULL)
        return NULL;

    int num = option->num_choices;
    int i;
    ppd_choice_t *choice = option->choices;

    //init object of paper
    jclass item = (*env)->FindClass(env, "com/android/printclient/objects/Item");
    jmethodID item_init = (*env)->GetMethodID(env, item, "<init>", "()V");
    jfieldID obj_choice = (*env)->GetFieldID(env, item, "choice", "Ljava/lang/String;");
    jfieldID obj_text = (*env)->GetFieldID(env, item, "text", "Ljava/lang/String;");
    jfieldID obj_marked = (*env)->GetFieldID(env, item, "marked", "Z");

    //init return list of papers
    jclass items = (*env)->FindClass(env, "java/util/ArrayList");
    jmethodID items_init = (*env)->GetMethodID(env, items, "<init>", "()V");
    jobject items_instance = (*env)->NewObject(env, items, items_init, "");
    jmethodID items_add = (*env)->GetMethodID(env, items, "add", "(Ljava/lang/Object;)Z");

    for (i = 0; i < num; ++i, choice++) {
        jobject *object = (*env)->NewObject(env, item, item_init, "");
        jstring item_choice = (*env)->NewStringUTF(env, choice->choice);
        jstring item_text = (*env)->NewStringUTF(env, choice->text);

        (*env)->SetBooleanField(env, object, obj_marked, (jboolean) choice->marked);
        (*env)->SetObjectField(env, object, obj_text, item_text);
        (*env)->SetObjectField(env, object, obj_choice, item_choice);

        (*env)->CallBooleanMethod(env, items_instance, items_add, object);
    }
    return items_instance;
}