= まえがき

== 本書の題名について

「Couchbase Mobile」とは、単体のソフトウェアを指す言葉ではなく、Couchbase LiteとSync Gatewayを包含する呼称です。
Couchbase Liteは、NoSQLドキュメント指向モバイル組み込みデータベースです。
Sync Gatewayは、Couchbase LiteとCouchbase Serverとのデータ同期を行うサーバーソフトウェアです。

また、「エッジコンピューティングデータプラットフォーム」については、エッジ端末におけるデータ管理に加え、エッジ上のデータとクラウド/中央データセンターのデータベースとの間の双方向のデータ同期機能を提供するプラットフォームという意味で使われています。

なお、書名としては語句が重複するため避けましたが、「モバイル/エッジコンピューティング〜」とする方が実情をうまく表しているため、本文中ではこの表現を用いています。

最後に、「エッジ」という言葉について補足します。
IoT(Internet of Things)隆盛の中、「エッジ」は、IoTデバイスの同義としても使われていますが、「エッジコンピューティング」というとき、それは「クラウドコンピューティング」への対比としての意味を持ちます。
その意味で「エッジ」という言葉は、モバイルやIoTデバイスのみならず、クラウド/中央データセンターに対する、「エッジ」データセンターという文脈でも用いられます。

== 本書の構成

本書は表現や内容、あるいは直接的に想定されている読者層について、傾向の異なるいくつかの部分からなります。

この「まえがき」に続く「プロローグ」は、二人の登場人物の対話の形式で書かれています。
ここでは、Couchbase Mobileについて、はじめてその名前を聞くという人を想定し、他のデータベースとの比較を行いながら紹介しています。

プロローグに続き、第1章はCouchbase Mobileの存在意義を伝えることを目的としています。

第２章から第12章は、Couchbase Liteを組み込みデータベースとして利用する際の基本的な情報を提供しています。
第２章と第３章でCouchbase Liteの基本機能について解説した上で、第４章にて各種プログラミング言語でCouchbase Liteを利用する方法を説明しています。
その後、Couchbase Liteを使ってアプリケーションを開発するための情報を提供するいくつかの章が続きます。
なお、第４章や第９章、第12章のように、個別のプログラミング言語に関する内容を扱っている章以外では、機能の解説やサンプルコードのためにAndroid Javaを用いています。

第13章から第20章は、Sync Gatewayについての解説に当られています。

第21章は、Couchbase Mobileを、Couchbase Serverクライアントアプリケーションと共に利用するケースについて解説しています。

第22章から第27章は、Couchbase LiteとSync Gatewayのデータ同期について、機能、内部機構から、設計や環境構築まで横断的な内容を扱っています。
特に第27章では、Couchbase Mobileの全体を体験することができるよう、サンプルアプリケーションを用いて、環境構築から操作までを、演習形式で解説しています。

最後の第28章では、それまでの内容を踏まえて、実際の開発に進む際に参考とすることのできる情報を紹介しています。

== 本書の記述対象

本書の記述内容は、Couchbase Mobile 3.0を対象としています。@<fn>{memo-blog-version}

Couchbase Mobileは、エンタープライズエディションとコミュニティーエディションの、ふたつの形態でバイナリが提供されており、その基盤としてオープンソースプロジェクト@<fn>{couchbase-oss}が存在しています。
本書の記述は、基本的にコミュニティーエディションに基づきます。
加えて、エンタープライズエディション独自の機能についても適宜紹介しています。@<fn>{memo-ee-usage}


//footnote[memo-blog-version][3.0リリース以前に発表されたブログから、現在でも有益と考えられる情報を紹介している箇所があります。]
//footnote[memo-ee-usage][また、第27章で紹介しているサンプルアプリケーションでは、エンタープライズエディション環境を利用しています。]
//footnote[couchbase-oss][https://developer.couchbase.com/open-source-projects/]


== リポジトリーについて

本書中に掲載しているサンプルコード等を以下のURLで公開しています。また、本書に関する情報共有のために、必要に応じ、このリポジトリーを更新する予定です。

//blankline

https://github.com/YoshiyukiKono/Couchbase_Mobile_First_Step_Guide.git

== 表記関係について

本書に記載されている会社名、製品名などは、一般に各社の登録商標または商標、商品名です。
会社名、製品名については、本文中では©、®、™マークなどは表示していません。

== 免責事項

本書の文責は著者にあり、所属する組織とは関係ありません。

また、本書に記載された内容は、情報の提供のみを目的としています。
正確かつ適切な情報を届けるため配慮を尽くしていますが、本書の内容の正確性、有用性等に関しては、一切保証するものではありません。
したがって、本書の情報を用いた開発、運用、コンサルティング等、いかなる実践も必ずご自身の責任と判断によって行ってください。
本書の情報を参照した行為の結果について、著者はいかなる責任も負いません。

== 底本について

本書籍は、2022年1月に開催された技術系同人誌即売会「技術書典12」で頒布された「エッジコンピューティングプラットフォームCouchbase Mobileファーストステップガイド」を底本とし、2022年2月にリリースされたCouchbase Mobile 3.0にあわせた内容の更新を行った他、全面的に改訂しています。