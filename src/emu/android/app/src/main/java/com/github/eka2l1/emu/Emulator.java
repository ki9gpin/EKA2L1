/*
 * Copyright (c) 2020 EKA2L1 Team
 *
 * This file is part of EKA2L1 project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

package com.github.eka2l1.emu;

import android.content.Context;
import android.graphics.Bitmap;
import android.os.Environment;
import android.view.Surface;

import com.github.eka2l1.BuildConfig;
import com.github.eka2l1.applist.AppItem;
import com.github.eka2l1.util.FileUtils;
import com.github.eka2l1.util.BitmapUtils;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;

import io.reactivex.Completable;
import io.reactivex.Single;

public class Emulator {
    public static final String EMULATOR_DIR = Environment.getExternalStorageDirectory() + "/EKA2L1/";
    public static final String COMPAT_DIR = EMULATOR_DIR + "compat/";

    public static final int INSTALL_DEVICE_ERROR_NONE = 0;
    public static final int INSTALL_DEVICE_ERROR_NOT_EXIST = 1;
    public static final int INSTALL_DEVICE_ERROR_INSUFFICENT = 2;
    public static final int INSTALL_DEVICE_ERROR_RPKG_CORRUPT = 3;
    public static final int INSTALL_DEVICE_ERROR_DETERMINE_PRODUCT_FAIL = 4;
    public static final int INSTALL_DEVICE_ERROR_ALREADY_EXIST = 5;
    public static final int INSTALL_DEVICE_ERROR_GENERAL_FAILURE = 6;
    public static final int INSTALL_DEVICE_ERROR_ROM_FAIL_TO_COPY = 7;
    public static final int INSTALL_DEVICE_ERROR_VPL_FILE_INVALID = 8;
    public static final int INSTALL_DEVICE_ERROR_ROFS_CORRUPTED = 9;
    public static final int INSTALL_DEVICE_ERROR_ROM_CORRUPTED = 10;
    public static final int INSTALL_DEVICE_ERROR_FPSX_CORRUPTED = 11;

    private static boolean init;
    private static boolean load;

    static {
        System.loadLibrary("native-lib");
    }

    public static void initializeFolders(Context context) {
        File folder = new File(EMULATOR_DIR);
        if (!folder.exists()) {
            folder.mkdirs();
        }
        File nomedia = new File(EMULATOR_DIR, ".nomedia");
        if (!nomedia.exists()) {
            try {
                nomedia.createNewFile();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        boolean shouldUpdate = checkUpdate();
        updateFolder(context, "resources", shouldUpdate);
        updateFolder(context, "patch", shouldUpdate);
        copyFolder(context, "compat", shouldUpdate);

        setDirectory(EMULATOR_DIR);
    }

    private static boolean checkUpdate() {
        boolean result = false;
        File versionFile = new File(EMULATOR_DIR, "version");
        String version = FileUtils.readFromFile(versionFile);
        if (!version.equals(BuildConfig.GIT_HASH)) {
            result = true;
            FileUtils.writeToFile(BuildConfig.GIT_HASH, versionFile);
        }
        return result;
    }

    private static void updateFolder(Context context, String folderName, boolean shouldUpdate) {
        File patchFolder = new File(EMULATOR_DIR, folderName);
        if (shouldUpdate || !patchFolder.exists()) {
            if (patchFolder.exists()) {
                FileUtils.deleteDirectory(patchFolder);
            }
            FileUtils.copyAssetFolder(context, folderName, EMULATOR_DIR + folderName);
        }
    }

    private static void copyFolder(Context context, String folderName, boolean shouldUpdate) {
        File patchFolder = new File(EMULATOR_DIR, folderName);
        if (shouldUpdate || !patchFolder.exists()) {
            FileUtils.copyAssetFolder(context, folderName, EMULATOR_DIR + folderName);
        }
    }

    public static Single<ArrayList<AppItem>> getAppsList() {
        return Single.create(emitter -> {
            checkInit();
            String[] apps = getApps();
            ArrayList<AppItem> items = new ArrayList<>();
            for (int i = 0; i < apps.length; i += 2) {
                long appUid = Long.parseLong(apps[i]);
                Bitmap[] iconAndMask = getAppIcon(appUid);

                Bitmap finalIcon = null;
                if (iconAndMask != null) {
                    if ((iconAndMask.length == 1) || (iconAndMask[1] == null)) {
                        finalIcon = iconAndMask[0];
                    } else {
                        finalIcon = BitmapUtils.SourceWithMergedSymbianMask(iconAndMask[0], iconAndMask[1]);

                        iconAndMask[0].recycle();
                        iconAndMask[1].recycle();
                    }
                }

                AppItem item = new AppItem(appUid, 0, apps[i + 1], finalIcon);
                items.add(item);
            }
            emitter.onSuccess(items);
        });
    }

    public static ArrayList<AppItem> getPackagesList() {
        String[] packages = getPackages();
        ArrayList<AppItem> items = new ArrayList<>();
        for (int i = 0; i < packages.length; i += 3) {
            AppItem item = new AppItem(Long.parseLong(packages[i]), Long.parseLong(packages[i + 1]), packages[i + 2], null);
            items.add(item);
        }
        return items;
    }

    public static Completable subscribeInstallDevice(String rpkgPath, String romPath, boolean installRPKG) {
        return Completable.create(emitter -> {
            int installResult = installDevice(rpkgPath, romPath, installRPKG);

            if (installResult == 0) {
                emitter.onComplete();
            } else {
                emitter.onError(new IOException(Integer.toString(installResult)));
            }
        });
    }

    private static void checkInit() {
        if (!init && load) {
            throw new IllegalStateException("Emulator is not initialized");
        }
        if (!init) {
            init = startNative();
            load = true;
            if (!init) {
                throw new IllegalStateException("Emulator is not initialized");
            }
        }
    }

    public static boolean isInitialized() {
        return init;
    }

    private static native void setDirectory(String path);

    private static native boolean startNative();

    private static native String[] getApps();

    public static native void launchApp(int uid);

    public static native void surfaceChanged(Surface surface, int width, int height);

    public static native void surfaceDestroyed();

    public static native void pressKey(int key, int keyState);

    public static native void touchScreen(int x, int y, int action);

    public static native boolean installApp(String path);

    public static native String[] getDevices();

    public static native void setCurrentDevice(int id);

    public static native void setDeviceName(int id, String newName);

    public static native int getCurrentDevice();

    public static native int installDevice(String rpkgPath, String romPath, boolean installRPKG);

    public static native boolean doesRomNeedRPKG(String romPath);

    public static native String[] getPackages();

    public static native void uninstallPackage(int uid, int extIndex);

    public static native void mountSdCard(String path);

    public static native void loadConfig();

    public static native void setLanguage(int languageId);

    public static native void setRtosLevel(int level);

    public static native void updateAppSetting(int uid);

    public static native Bitmap[] getAppIcon(long uid);

    public static native String[] getLanguageIds();

    public static native String[] getLanguageNames();
}
