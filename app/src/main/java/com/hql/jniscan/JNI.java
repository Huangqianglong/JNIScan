package com.hql.jniscan;

import android.util.Log;

import java.util.ArrayList;

/**
 * @author ly-huangql
 * <br /> Create time : 2019/7/3
 * <br /> Description :
 */
public class JNI {
    ArrayList<MusicBean> dataList = new ArrayList<>();
    public native void scanDir(String path);

    public void printMediaPath(String path){
        Log.d("hql","JNI 内输出媒体路径>>>>"+path);
        MusicBean bean = new MusicBean();
        bean.setPath(path);
        dataList.add(bean);
    }
    public int getCacheSize(){
        return dataList.size();
    }
    public void clearCatch(){
        dataList.clear();
    }
    class MusicBean {
        String path;

        public String getPath() {
            return path;
        }

        public void setPath(String path) {
            this.path = path;
        }
    }
}
