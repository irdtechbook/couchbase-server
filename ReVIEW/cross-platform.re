= クロスプラットフォーム開発

== 概要

Couchbase Liteを用いたモバイルアプリを開発する際に、iOSとAndroidの両方で利用するために、クロスプラットフォーム@<fn>{Cross-platform_software}/ハイブリッドアプリ@<fn>{hybrid-application}開発に関連する内容を整理します。


以下、ここで扱うフレームワークについて、概要を整理します。

//footnote[Cross-platform_software][https://en.wikipedia.org/wiki/Cross-platform_software]
//footnote[hybrid-application][https://www.techtarget.com/searchsoftwarequality/definition/hybrid-application-hybrid-app]

//blankline

 * @<em>{Xamarin}: C#で書かれたコードが、Intermediate Language(IL)としてコンパイルされます。
 * @<em>{Flutter}: Dartで書かれたコードが、C/C++コードとしてコンパイルされて、ネイティブに実行されます。
 * @<em>{React Native}: JavaScriptやTypeScriptで書かれたコードが、JavaScriptエンジンで実行されます。
 * @<em>{Ionic}: JavaScriptやHTML、CSSのようなWebアプリケーション開発技術を使って書かれたコードが、Webブラウザーの機能を提供するWeb View@<fn>{webview}を介して、ネイティブアプリやPWA(Progressive Web Apps)として実行されます。

//footnote[webview][https://ionicframework.com/docs/ja/core-concepts/webview]

== Xamarin

=== 概要


Couchbase LiteをXamarinアプリケーション開発に使う方法を、Couchbase Labsで公開されているサンプルアプリケーションを使って解説します。
このサンプルアプリケーションについて公式チュートリアル@<fn>{userprofile-standalone-xamarin}が公開されています。

//footnote[userprofile-standalone-xamarin][https://docs.couchbase.com/tutorials/userprofile-standalone-xamarin/userprofile_basic.html]

サンプルアプリケーションは、Xamarin.Forms、C#、XAML(Extensible Application Markup Language)を使用しています。
iOS、Android、およびUWPに対応しており、それぞれ以下の環境を利用することが想定されています。

//blankline

 * iOS (Xcode 12.5+)
 * Android (SDK 22+)
 * UWP (Windows 10)


=== サンプルアプリケーション

ここで紹介するアプリケーションは、Couchbase LiteをCouchbase Serverとの同期を行わずにスタンドアローンで利用するものです。

アプリケーションを実行するには、以下のようにGitHubリポジトリーから@<tt>{standalone}ブランチをクローンします。


//cmd{
$ git clone -b standalone \
  https://github.com/couchbaselabs/userprofile-couchbase-mobile-xamarin.git
//}

本アプリケーションは、ユーザープロファイル情報管理機能を提供します。
プロファイル情報は、Couchbase Liteで管理されます。

ログイン後、そのユーザーのプロファイル情報を表示・編集することができます。
ログイン画面にて情報したユーザーが未登録の場合は新規ユーザーとして追加され、ログイン画面で入力した情報を引き継がれたプロファイル画面に遷移後、追加の情報を入力します。

=== ソリューション構造

ソリューションは、7つのプロジェクトで構成されています。

//blankline

 * UserProfileDemo: ビュー機能のための.NET標準プロジェクト
 * UserProfileDemo.Core: ビューモデル機能のための.NET標準プロジェクト
 * UserProfileDemo.Models: データモデルのための.NET標準プロジェクト
 * UserProfileDemo.Repositories: データベースの管理のためのリポジトリークラスを含む.NET標準プロジェクト
 * UserProfileDemo.iOS: .ipaファイルのビルドを担当するiOSプラットフォームプロジェクト
 * UserProfileDemo.Android: .apkファイルのビルドを担当するAndroidプラットフォームプロジェクト
 * UserProfileDemo.UWP: .exeファイルのビルドを担当するUWPプラットフォームプロジェクト

//blankline

@<tt>{/modules/userprofile/examples/src}の下に、ソリューションファイルUserProfileDemo.slnが存在します。

Couchbase.Liteパッケージは、このソリューションの以下の4つのプロジェクト内で利用されています。

//blankline

 * UserProfileDemo.Repositories
 * UserProfileDemo.iOS
 * UserProfileDemo.Android
 * UserProfileDemo.UWP


=== Couchbase Liteアクティブ化

Xamarinアプリ内でCouchbase Liteを使用するには、Couchbase.Liteをプラットフォームごとにアクティブ化します。

以下、各プラットフォームにおける例を示します。

//blankline

iOSプラットフォームの場合。

//emlist[AppDelegate.cs][csharp]{
Couchbase.Lite.Support.iOS.Activate();
//}

Androidプラットフォームの場合。

//emlist[MainActivity.cs][csharp]{
Couchbase.Lite.Support.Droid.Activate(this);
//}

UWPプラットフォームの場合。

//emlist[MainPage.xaml.cs][csharp]{
Couchbase.Lite.Support.UWP.Activate();
//}


チュートリアルでは、アプリケーションで利用されるデータモデルの説明や、データベース操作について機能毎に解説が行われています。


=== 開発参考情報


ここで紹介したチュートリアルの発展として、データベースに対するクエリを扱ったもの(User Profile Sample: Couchbase Lite Query Introduction@<fn>{userprofile_query})や、Couchbase Serverとのデータ同期を扱ったもの(User Profile Sample: Data Sync Fundamentals@<fn>{userprofile_sync})が公開されています。


//footnote[userprofile_query][https://docs.couchbase.com/tutorials/userprofile-query-xamarin/userprofile_query.html]


//footnote[userprofile_sync][https://docs.couchbase.com/tutorials/userprofile-sync-xamarin/userprofile_sync.html]


== Flutter

=== 概要


Flutterは、FFI(foreign function interface@<fn>{Foreign_function_interface})による連携をサポートしており、プラットフォームネイティブコードとのインテグレーションに用いることができるdart:ffi@<fn>{flutter-c-interop}パッケージを提供しています。
そのため、Couchbase Lite C API@<fn>{couchbase-lite-C}を用いてCouchbase LiteをDart/Flutterアプリケーションで利用することができます。
そのような試みとして、以下で紹介するパッケージが公開されています。

//footnote[flutter-c-interop][https://docs.flutter.dev/development/platform-integration/c-interop]

//footnote[Foreign_function_interface][https://en.wikipedia.org/wiki/Foreign_function_interface]

//footnote[couchbase-lite-C][https://github.com/couchbase/couchbase-lite-C]

=== Couchbase Liteパッケージ


DartとFlutterの公式パッケージリポジトリー(pub.dev@<fn>{pub-dev})に、各種パッケージが公開されています。

Couchbase Liteに関するパッケージは、アクティブにメンテナンスされていないように見えるものも含めて、何種類か存在します。
ここでは、現在最もアクティブと思われるパッケージを紹介します。

以下は、同じPublisherによって開発された一連のパッケージです。目的に合わせた組み合わせにより利用することが想定されています。

//footnote[pub-dev][https://pub.dev/]

//blankline

 * cbl@<fn>{cbl}: Couchbase Lite APIのパッケージです。以下のパッケージを利用する際、同時に利用します。
 * cbl_dart@<fn>{cbl_dart}: サーバーやCLI等、純粋なDartアプリケーション用のパッケージです。
 * cbl_flutter@<fn>{cbl_flutter}: Flutterアプリケーション開発用のパッケージです。

//blankline

//footnote[cbl_dart][https://pub.dev/packages/cbl_dart]
//footnote[cbl][https://pub.dev/packages/cbl]
//footnote[cbl_flutter][https://pub.dev/packages/cbl_flutter]

パッケージの利用方法や、サンプルアプリケーションについて、各パッケージのサイトを参照することができます。


== React Native

=== 概要

React Native@<fn>{reactnative-dev}は、ハイブリッドアプリ開発のためのオープンソースUIフレームワークです。

React Nativeは、Native Module@<fn>{native-modules-intro}システムを要しており、AndroidとiOSそれぞれのネイティブ実装(Android JavaとObjective-C/Swift)と組み合わせてアプリケーションを開発する手段を提供しています。

=== Couchbase Lite React Native Module

Couchbase Labsにて、Couchbase LiteをReact Nativeで利用するためのReact Native Moduleの参照実装(couchbase-lite-react-native-module@<fn>{couchbase-lite-react-native-module})が公開されています。
このモジュールの位置づけについて、以下の引用を参照ください。


//quote{
NOTE: The plugin is not officially supported by Couchbase and there are no guarantees that the APIs exported by the module are up to date with the latest version of Couchbase Lite. The module implementation is available as an open source reference implementation for developers to use as a starting point and contribute as needed
//}

上記の翻訳を以下に示します。

//quote{
注: このプラグインはCouchbaseによる公式サポートではなく、モジュールによってエクスポートされているAPIが最新バージョンのCouchbase Liteに追随しているという保証はありません。モジュールの実装は、開発者がスタート地点として使用でき、また必要に応じて貢献することのできるオープンソース参照実装として公開されています。
//}

この参照実装は、ネイティブCouchbase Lite API機能のサブセットをエクスポートし、ReactネイティブJavaScriptアプリで利用できるようにしています。
プラグインによってJavaScriptにエクスポートされたAPIのリストや個々のAPI使用例について、リポジトリーのREADMEを参照することができます。

また、このモジュールを使用したサンプルアプリケーション@<fn>{userprofile-couchbase-mobile-reactnative}も公開されています。



//footnote[native-modules-intro][https://reactnative.dev/docs/native-modules-intro]
//footnote[couchbase-lite-react-native-module][https://github.com/couchbaselabs/couchbase-lite-react-native-module]
//footnote[userprofile-couchbase-mobile-reactnative][https://github.com/couchbaselabs/userprofile-couchbase-mobile-reactnative]

//footnote[reactnative-dev][https://reactnative.dev/]

このような既存のモジュールを利用しない場合、あるいは既存のモジュールを拡張する場合には、Couchbase LiteのネイティブAPIにアクセスするために、React Nativeが提供するNative Moduleシステムを利用して実装を行います。


== Ionic

=== 概要

Ionic@<fn>{ionicframework}は、ハイブリッドアプリ開発のためのオープンソースUIフレームワークです。
JavaScriptや、HTML、CSSといったWebアプリケーション開発技術を使って、ネイティブアプリやPWA(Progressive Web Apps)@<fn>{pwa}を開発するために利用することができます。
Angular、React、そしてVueのようなJavaScriptのWebフレームワークと組み合わせて利用されます。

Ionicフレームワークは、GitHubにおいて、Ionicチーム(https://github.com/ionic-team)によってリポジトリー@<fn>{github-ionic-framework}が公開されています。

ハイブリッドモバイルアプリ開発のために必要となるネイティブ連携のフレームワークとして、同じくIonic(Ionic.io@<fn>{ionic})によるCapacitor@<fn>{capacitorjs}があります。
Ionic.ioは、Ionicの有償サポートも提供しています。@<fn>{ionic-pricing}


//footnote[apache-cordova][https://cordova.apache.org/]
//footnote[github-ionic-framework][https://github.com/ionic-team/ionic-framework]
//footnote[ionicframework][https://ionicframework.com/]
//footnote[cordova-to-capacitor][https://capacitorjs.com/cordova]
//footnote[cordova-build][https://ionicframework.com/docs/cli/commands/cordova-build]

=== Couchbase Lite統合

Ionic開発に、Couchbase Liteを利用するための、IonicのCouchbase Lite統合(Ionic's Couchbase Lite integration@<fn>{ionic-couchbase-lite})が提供されています。
このIonic Couchbase Lite統合のネイティブ連携レイヤーには、Capacitorが利用されています。

Couchbase Lite統合を利用するには有償のサポートが必要とされますが、パッチのリリースや新機能のアップデート、さらにiOSおよびAndroidの新しいリリースに対する互換性維持など、継続的なアップデートとメンテナンスが謳われています。

なお、IonicのCouchbase Lite統合を利用するには、Couchbase Liteエンタープライズエディションが必要です。


//footnote[ionic][https://ionic.io/]
//footnote[ionic-pricing][https://ionic.io/pricing]
//footnote[capacitorjs][https://capacitorjs.com/]

//footnote[pwa][https://en.wikipedia.org/wiki/Progressive_web_application]

//footnote[ionic-couchbase-lite][https://ionic.io/docs/couchbase-lite]


//footnote[hotel-lister-cordova][https://github.com/couchbaselabs/hotel-lister-cordova]
//footnote[hotel-lister-ios][https://docs.couchbase.com/tutorials/hotel-lister/ios.html]
//footnote[hotel-lister-android][https://docs.couchbase.com/tutorials/hotel-lister/android.html]


====[column]Apache Cordova

Apache Cordova@<fn>{apache-cordova}は、JavaScript、HTML、CSSといったWebアプリケーション開発技術を使って、モバイルアプリケーションを開発することができるフレームワークです。
Cordova Native APIによりネイティブ連携を行う手段が提供されています。

Ionicは、Cordovaとの組み合わせで、AndroidおよびiOSアプリを開発する手段(@<tt>{ionic cordova build}@<fn>{cordova-build})を提供しています。
一方で、CordovaからCapacitorへのマイグレーション(Cordova to Capacitor Migration@<fn>{cordova-to-capacitor})の方法が公開されており、そこではCapacitorについて「最新の開発体験とCordovとの99%の下位互換性(A modern development experience and 99% backward-compatibility with Cordova)」と説明されています。

====[/column]