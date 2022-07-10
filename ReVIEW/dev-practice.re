
= 開発の実践に向けて

本書によって、Couchbase Mobileを使った開発について、基本的なイメージを掴むことができたと感じていただけたのなら幸いです。

一方、当然のことながら、アプリケーション開発は、一冊の書籍で語り尽くせてしまえるものではありません。

ここでは、さらに学習を進めるにあたって有益と思われる情報を紹介し、締め括りとしたいと思います。

== コミュニティ

Couchbase Dev Communityサイト@<fn>{developer-couchbase}では、Couchbase開発者コミュニティへ向けて、チュートリアルやベストプラクティス等、様々な情報提供が行なわれています。
また、フォーラム@<fn>{forums-couchbase}で、コミュニティエディションに関する疑問について質問したり、過去に行なわれた質疑応答を検索したりすることができます。

== 無償オンライントレーニング

Couchbase, Inc.が運営するCouchbase Academyでは、各種のトレーニングコースや認証資格が提供されています。
自分のペースで受講することができる、Couchbase Mobileについての無償のオンライントレーニングとして、「CB040: Essentials of Couchbase Mobile and IoT@<fn>{achademy-mobile-course}」が存在します。

その他、AndroidやiOSを対象としたCouchbase Mobileのコースも存在します。
提供されるコースは変化する可能性があります。
Couchbase Academyのトップページ@<fn>{couchbase-academy}から、ご確認ください。

== ブログ

本書中でもいくつかの記事を紹介しましたが、Couchbase Blog@<fn>{couchbase-blog}には、アークテクチャーの解説や、アプリケーション構築や操作の具体例、新機能の紹介など様々な記事が公開されています。

ここでは、Couchbase Mobileに関する以下の記事を紹介します。

//blankline

 * Couchbase Mobile changes source code license to BSL 1.1@<fn>{couchbase-mobile-changes-source-code-license-to-bsl-1-1}

== ドキュメント、APIリファレンス

Couchbase LiteのドキュメントやAPIリファレンスについては、各プログラミング言語用のランディングページからアクセスできます。
URLについては、すでに開発参考情報として紹介しているため、ここでは割愛します。

== チュートリアル、サンプルアプリケーション

Couchbaseチュートリアル(Couchbase Tutorials@<fn>{couchbase-totorials})から、モバイル開発者向けのチュートリアルを見つけることができます。
また、Couchbase Labs GitHubリポジトリ@<fn>{github-couchbaselabs}では、Couchbaseに関連する様々なプロジェクトが公開されており、Couchbase Mobileを使ったサンプルアプリケーションも含まれています。
これらについて、すでにその中からいくつかを開発参考情報として紹介しています。

//footnote[couchbase-totorials][https://docs.couchbase.com/tutorials/index.html]

== ソースコード

Couchbaseは、様々なプロジェクトをオープンソースとして公開しており、ソースコードやIssue Trackerなどにアクセスすることができます。

ここでは、Couchbase Lite Androidプロジェクトについて説明します。

=== Open Source Projects on Couchbase

オープンソースプロジェクトのランディングページ(Open Source Projects on Couchbase@<fn>{open-source-projects})から現時点でリンクされているCouchbase Lite Androidプロジェクト(couchbase-lite-android@<fn>{couchbase-lite-android})は、「This repository is deprecated as of Couchbase Lite v2.8.」とされており、別のプロジェクト(couchbase-lite-java-ce-root@<fn>{couchbase-lite-java-ce-root})へのリンクが掲載されています。

=== Couchbase Lite Java CE Root

名称の違いにも表現されていますが、このプロジェクトは「About The root workspace for the Community Editions of the Java language family of products (Java Desktop, Java WebService, and Android)」とされています。

このプロジェクト中には、「ce..」、「common...」、「core...」という名称のリンクが存在しています。
「common...」という名称でリンクされているプロジェクト(couchbase-lite-java-common@<fn>{couchbase-lite-java-common})、から、基本的なJavaのソースコードを参照することができます。

=== Couchbase Lite Java Common

このプロジェクトは「Common code for the Java language family of products (Java Desktop, Java WebService, and Android)」とされています。

ソースコードは、「common」、「android」、「java」等のフォルダーに分かれて格納されています。
プラットフォームに依存しない大部分のソースコードについて、「common」以下を参照することになります。

注意しなければならないところとしては、この「common」以下のパッケージの@<tt>{com.couchbase.lite}パッケージに含まれるクラスのソースコードが格納されているパス@<fn>{couchbase-lite-java-common-com-couchbase-lite}を確認した時に、ログ関連のクラスのようなプラットフォーム依存のクラスではないにもかかわらず、存在しないファイルがあることです。
例えば、@<tt>{AbstractReplicatorConfiguration.java}は存在しますが、@<tt>{ReplicatorConfiguration.java}は存在していません。
この@<tt>{ReplicatorConfiguration.java}ファイルを確認するには、別のプロジェクト(couchbase-lite-java-ce@<fn>{couchbase-lite-java-ce})にあたることになります。

=== Couchbase Lite Java CE

このプロジェクトは、「Code for the Community edition of the Java language family of products (Java Desktop, Java WebService, and Android)」とされています。

@<tt>{ReplicatorConfiguration}や、@<tt>{Database}、@<tt>{Function}、@<tt>{IndexBuilder}クラスのようなCE(コミュニティエディション)とEE(エンタープライズエディション)で機能に差異があるクラスのソースコードが、このプロジェクトに含まれています。
なおエンタープライズエディションのソースコードは一般に公開されていませんが、利用可能なクラスやメソッドについては、APIリファレンス@<fn>{couchbase-lite-android-api-ref}から参照可能です。

APIリファレンスの内容と、このプロジェクトに格納されているソースコードの内容を対照することによって、クラスとメソッドのレベルで、コミュニティエディションとエンタープライズエディションの機能差異を確認することができます。




//footnote[couchbase-blog][https://blog.couchbase.com/]

//footnote[couchbase-lite-android][https://github.com/couchbase/couchbase-lite-android]
//footnote[couchbase-lite-java-ce-root][https://github.com/couchbase/couchbase-lite-java-ce-root]

//footnote[couchbase-lite-java-common][https://github.com/couchbase/couchbase-lite-java-common]

//footnote[couchbase-lite-java-common-com-couchbase-lite][https://github.com/couchbase/couchbase-lite-java-common/tree/android/release/3.0/common/main/java/com/couchbase/lite]


//footnote[couchbase-lite-java-ce][https://github.com/couchbase/couchbase-lite-java-ce]

//footnote[open-source-projects][https://developer.couchbase.com/open-source-projects/]

//footnote[github-couchbaselabs][https://github.com/couchbaselabs/]
//footnote[developer-couchbase][https://developer.couchbase.com/]
//footnote[forums-couchbase][https://forums.couchbase.com/]

//footnote[couchbase-academy][https://www.couchbase.com/academy]
//footnote[achademy-mobile-course][https://learn.couchbase.com/store/404628-cb040-essentials-of-couchbase-mobile-and-iot]

//footnote[couchbase-lite-android-api-ref][https://docs.couchbase.com/mobile/3.0.0/couchbase-lite-android/]

//footnote[couchbase-mobile-changes-source-code-license-to-bsl-1-1][https://blog.couchbase.com/couchbase-mobile-changes-source-code-license-to-bsl-1-1/]
//footnote[couchbase-2022-predictions][https://blog.couchbase.com/couchbase-2022-predictions-technology-adoption-trends-will-improve-developer-agility-and-create-dynamic-and-interactive-cx-in-2022/]