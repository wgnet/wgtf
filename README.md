Wargaming Tools Framework
=========================
WGTF stands for "Wargaming Tools Framework" It is a global tools initiative to
rebuild Wargaming tools on a new framework that is driven by a plug-in based
architecture. This architecture would allow for greater flexibility in sharing
code and common functionality across multiple projects as well as supporting
cross-platform development. The project uses a combination of C++ and Qt.

Introduction
------------
This guide is for building and running the WGTF test app.

Step-by-Step Guide
------------------

After pulling down the repository, you will need to add several dependancies.
The version of Qt you plan to use must be in the wgtf/src/core/third_party/Qt
directory and cmake must be in the wgtf/src/core/third_party/cmake directory


### On Windows

1.  Run **wgtf/build/wg_cmake.bat**

    a. Select **Visual Studio [version] [arch] - [Qt version]**

    The version of Qt depends on the age of the branch. As of writing **VS2012,
    Win64, Qt 5.6.0** is used on **develop**. **VS2013** and **Win32** should
    also work. The **deploy** targets are for packaging relevant sources into the
    **bin** folder to give to other people. E.g. QA or consumers. The **deploy**
    target does not require a git clone, only **bin**. The **non-deploy**
    targets should be used for development.

    b. The solution will be generated to
    **wgtf/build_generic_app_test_vc11_win64_qt5.6.0/generic_app_test_win64.sln**

    If you want an out-of-tree build, pass in the desired output path to
    **wg_cmake.bat** E.g. Edit **wg_cmake.bat**

```
    bat
    @python wg_cmake.py D:/build/wgtf
    @pause
```

```
    Qt and cmake are required to build WGTF. Read wgtf/src/core/third_party/qt_readme.txt and
    wgtf/src/core/third_party/cmake_readme.txt for instructions on using them.

    Some projects will be ignored if 3rd party libraries are not available.
    Add any other 3rd party dependencies if needed. For Python support, follow
    the instructions in wgtf/src/core/third_party/python_readme.txt, for Perforce
    support, read wgtf/src/core/third_party/perforce_readme.txt
```

2. Open the generated solution (**generic_app_test_win64.sln**) in _Visual
Studio 2012 Update 4._

3. Selet the **Hybrid** (release) build configuration

4. Go to **BUILD -> Build Solution** or press **F7**.

5. In the **Solution Explorer**, right click on the Executables/generic_app
project and select **Set As Startup Project**.

6. Select the plugins config you want to run. The plugins configs can be found
in **wgtf/bin/generic_app_test/win64/plugins**. For this example we will use
**plugins_ui.txt**.

7. In the **Solution Explorer**, right click on the **Executables/generic_app**
project and select **Properties**

8. In the **generic_app Property Pages** dialog, find **Configuration Properties
-> Debugging -> Command Arguments** and enter

```
    --config plugins_ui.txt
```

9. Press **OK**

10. Select **Debug -> Start Debugging** or press **F5**.

11. If it asks about building **ZERO_CHECK** press **OK**

```
    Building ZERO_CHECK basically re-runs the CMake script with some cached
    settings. You can build it if you have added/deleted a file and want to
    update the sln.  Sometimes Visual Studio isn't very good at detecting
    if it should build it.
```

12. The Generic App UI Test should start. It has all of the features core to
the WGTF framework.

13. Log output should appear in the Visual Studio Output window. To view logs
without debugging, open
[Debug View](https://technet.microsoft.com/en-us/sysinternals/debugview.aspx)




### On OSX

1. Run **wgtf/build/wg_cmake.sh**

    a. Select **XCode**

    b. Select **[Qt version]**

    The version of Qt depends on the age of the branch. As of writing Qt
    **5.6.0** is used on **develop**.

    c. Select the **non-deploy** target. The **deploy** targets are for
    packaging relevant sources into the **bin** folder to give to other people.
    E.g. QA or consumers. The **deploy** target does not require a git clone,
    only **bin**. The **non-deploy** targets should be used for development.

    d. The Xcode project will be generated to
    **wgtf/build_generic_app_test_xcode_qt5.6.0/generic_app_test_mac.xcodeproj**
    (depending on the selected Qt version).

```
    Qt and cmake are required to build WGTF. Read wgtf/src/core/third_party/qt_readme.txt and
    wgtf/src/core/third_party/cmake_readme.txt for instructions on using them.

    Some projects will be ignored if 3rd party libraries are not available.
    Add any other 3rd party dependencies if needed. For Python support, follow
    the instructions in wgtf/src/core/third_party/python_readme.txt, for Perforce
    support, read wgtf/src/core/third_party/perforce_readme.txt
```

2. Open the generated solution (**generic_app_test_mac.xcodeproj**) in _OSX El
Capitan 10.11.1 Xcode Version 7.1 (7B91b)_ by double clicking it in the finder.

3. Select the **Automatically Create Schemes** at the prompt.

4. Select the **ALL_BUILD** scheme from the **Scheme menu** in the toolbar at
the top.

5. Make sure the target is **My Mac (64-bit)**.

    a. The target might fail to build if the wrong SDK is selected. Follow these
    steps if it says "The run destination My Mac is not valid" when you go to
    build.

    b. Double-click on the **generic_app_test_mac** project in the **Navigation
    area** on the left of the window.

    c. Select **Build Settings** and the **All** and **Levels** filters.

    d. Make sure **Base SDK** is set to **Latest OS X (OS X 10.11)** for
    **ALL_BUILD** and **generic_app_test**

6. Press **CMD+B** or select **Product->Build** to build.

7. Change schemes to **generic_app**

8. Find the plugins config you want to run. The plugins configs can be found in
**wgtf/bin/generic_app_test/mac/generic_app.app/Contents/Resources/plugins**
(Show Package Contents in Finder). For this example we will use **plugins_ui.txt**.

9. Edit the scheme

    a. In the **Product|Scheme** menu, scroll down to the bottom, **Edit Scheme...**

    b. In the **Edit Scheme** dialog, find **Run -> Arguments -> Arguments ->
    Arguments Passed On Launch** and enter
```
    --config plugins_ui.txt
```

10. Press **Close**.

11. Start debugging by pressing the little **play icon** in the toolbar next to the
scheme.

12. If it asks for permissions, get permissions.

13. The Generic App UI Test should start. It has all of the features core to
the WGTF framework.


Maya Plugin
-----------

Building the Maya Plug-in

1. Go to the wgtf/build folder.

2. Double click on wg_cmake.bat.

3. Select maya_plugin -> Visual Studio 2012 Win64 -> Maya 2014.

4. Go to the build folder located in wgtf.

5. Select the solution file and open with VS2012.

6. Build the Hybrid solution.


Running the Maya Plug-in

1. Start Maya 2014.

2. Go to window -> settings/preference -> Plug-in Manager.

3. Select Browse at the bottom of the Plug-in Manager window.

4. Browse to wgtf/bin/generic_app_test/win64 folder and select wgtf_maya_plugin.mll

5. Select Open


The WGTF Maya plug-in should open on the right side of the viewport.

The behavior of WGTF Maya Plugin should be the same as WGTF stand alone
Generic_app.exe --config plugins_ui.txt. The features and panels should all
work as in the stand-alone version.
