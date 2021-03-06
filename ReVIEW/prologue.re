= プロローグ: モバイル/エッジデータベース選択を巡る対話


「先輩、すみません。少しお時間いただけますか？相談に乗ってもらいたいことがあるのですが」

「もちろん。どんな相談ですか？」

「実は、今後のモバイルアプリケーション開発で使う、組み込みデータベースの選択肢を整理しようとしているんですが…」

「AndroidやiOSアプリ用のデータベースという理解でよいのかな?」

「はい。それから、モバイルアプリだけでなくエッジデバイスも視野に入れて整理できればと思っています」

「了解。調査は、どの程度まで進んでいますか？」

「調べ始めたばかりで、まだ資料など作っていないため、お見せしながら話すことはできないんですが…」

「それは大丈夫。どの程度調べたのか聞かせてもらえますか？」

「ありがとうございます。まず、SQLiteについては外せないと思っています」

「うん。それから？」

「後は、iOSのCore DataやAndroidのRoom、それから、こうしたプラットフォーム固有の技術以外の選択肢として、Realmを取り上げようと思っています。モバイル用データベースは他にもあると思うんですが、その点で知恵を貸していただければと思って、相談させていただきました」

「わかりました。今後、整理するにあたって気にかけている部分はありますか？」

「MBaaS (Mobile Backend as a Service)について、別途主要クラウドプロバイダーのサービスをまとめようと思っているのですが、組み込みデータベースについても、クラウドやデータセンターのデータベースと同期する方法について整理したいと思っています」

「Realmについてはある程度調べていそうですね」

「まだ調べきれてはいませんが、RealmがMongoDBと同期できるということまではわかりました。MongoDBはNoSQLデータベースですよね。それで、他のNoSQLデータベースでも同じような機能を持ったものがあるかもしれないと思ったんです」

「なるほど、それで私に声をかけたということですね」

「はい、先輩はNoSQLデータベースについてご経験があると聞いたので」

「では、今聞いた点を踏まえて話しますね。まず、データベースの種類については、どの程度意識されていますか?一口にNoSQLといっても色々な種類がありますが」

「MongoDBは、ドキュメント指向データベースですよね。RealmもリレーショナルDBのようなテーブル形式でないデータを扱うという意味で、同じNoSQL同士という関係だと思っています」

「そういう理解ですか。では、NoSQLについて話す前に、そもそもCore DataやRoomを利用する意味はどこにあると思いますか?」

「漠然とした言い方かもしれませんが、SQLiteの制約を乗り越える、より進んだ選択肢ということだと思います」

「それでは、SQLiteの制約とは何ですか?」

「一面的な見方かもしれませんが、SQLクエリの結果をアプリケーションのデータモデルに変換するためのコードを自分で書かなければならないところだと思います」

「いわゆるオブジェクトとリレーショナルのインピーダンスミスマッチ(Object-relational impedance mismatch@<fn>{wiki-impedance_mismatch})と呼ばれるものですね。モバイルに限らず、データベース一般において、ORM(Object-relational mapping@<fn>{wiki-orm})フレームワークが解決する課題ともいえますね」


//footnote[wiki-impedance_mismatch][https://en.wikipedia.org/wiki/Object%E2%80%93relational_impedance_mismatch]
//footnote[wiki-orm][https://en.wikipedia.org/wiki/Object%E2%80%93relational_mapping]

「はい。Javaでリレーショナルデータベースを使った開発の経験があるので、その点については理解しています」

「SQLクエリは実行時に評価されるので、コンパイル時には検証されないのに対して、コンパイル時チェックによるタイプセーフなコードを開発することができる、という面から捉えることもできますね」

「わかります」

「そこからRealmについて考えてみると、どういう共通点、または違いがあるでしょう?」

「RealmはNoSQLだから、SQLクエリの結果からオブジェクトに変換する必要がないですよね」

「間違ってはいませんが、NoSQLだからというのは不正確です。そもそもNoSQLという言葉が、技術的な特徴について具体的に語るためには不十分です」

「この場合は、ドキュメント指向データベース、ということですよね」

「そう言うのも理解できますが、ここには誤解があります」

「そうなんですか？」

「そもそも、ドキュメント指向データベースとは何でしょうか？」

「JSONデータを扱うデータベースと理解しています」

「はい、広い意味ではドキュメント指向という言葉は、JSONだけでなくXMLのようなデータを含みますが、MongoDBや多くのドキュメント指向データベースはJSONデータを扱っています」

「じゃあ、やっぱりドキュメント指向データベースという整理でよいのではないですか？」

「どこから話そうか。まず、歴史的に言うと、Realmは2019年にMongoDBに買収されましたが、それ以前にも十分なシェアを持っていたといえます。日本でもRealmに関する書籍が出版されていたり、モバイルアプリ開発用データベースとしてSQLiteと共に紹介されていたりしました。だから、その頃から使っている人には誤解はないと思いますが、Realmはドキュメント指向ではなくオブジェクト指向のデータベースです。公式ページでも、そのようにはっきり記されています」

「そうなんですね。違う種類というのはわかりましたが、実際どういう違いなんでしょうか？」

「Javaの経験があるということだから、オブジェクト指向はわかりますよね?」

「はい。クラスを中心としたアプリケーション開発、というと色々端折りすぎかもしれませんが…」

「ここでの話としては、それで十分です。つまり、Realmを使う際にはクラス、つまりデータスキーマをあらかじめ定義する、ということです。そのスキーマは、RDBのテーブルスキーマより構造的に柔軟といえますが、それでも格納されるデータの定義が事前に必要であることに変わりはありません。一方、ドキュメント指向、つまりJSONデータベースはどうかというと…」

「そうか、データの構造は、JSONデータそのもので定義されるから、事前のスキーマ定義は不要」

「そう、それがスキーマレスとも言われるドキュメント指向データベースの特徴ですね。それはMongoDBについても当てはまりますが、Realと同期する際には、事前にデータスキーマを定義することになります」

「そういうことなんですね。MongoDBはドキュメント指向データベースだけど、Realmはそうではなく、オブジェクト指向データベースということですね」

「混乱するのも仕方がないかとも思います。データ構造において、SQLやRDBが扱うテーブルにはない柔軟性がある、というところまでは共通していますし、だからこそRealmとMongoDBとのデータ同期が成立する訳ですが、事前のデータ定義の必要性の有無という面で、ドキュメント指向とオブジェクト指向の違いは理解しておく意味があると思います」

「はい」

「この違いそれ自体は、優劣ではなく、性質の違いといえるでしょう。RealmはそもそもMongoDBのフロントエンドとして開発されたわけではなく、単体の組み込みデータベースとして開発されたものなので、アプリケーション内で定義されたデータを保存するという観点からは、データモデルの定義を前提とするオブジェクト指向という性質は自然、という見方もできると思います」

「事後的にMongoDBとの連携が行われたことから、私のようなよく知らない者にとって、誤解の余地が生じているということですね」

「そこで、本来私に期待されていた、データベースの紹介ということで言えば、ドキュメント指向の組み込みデータベースも存在しています」

「あ、そうなんですね」

「それは、Couchbase Liteというデータベースです」

「JSON形式のデータを扱う組み込みデータベース、という理解でよいですか?」

「その通りです」

「なるほど。それは是非、将来のプロジェクトの選択肢として資料に加えたいと思います。自分でも調べてみますが、少し教えていただいてよいですか？」

「もちろん。背景や位置づけについて把握してから調べた方が効果的だと思うので、説明しますね」

「お願いします」

「まず、データベースの名前はCouchbase Liteと言いましたよね。Liteがついているからには、LiteではないCouchbaseもあると考えるのが自然だと思いますが、実際Couchbase Serverというデータベースがあります。Couchbase Serverは、ドキュメント指向NoSQLデータベースで、JSONデータを扱います」

「Couchbase Serverのライト版がCouchbase Liteということですね」

「あくまで名前の由来という意味でね。NoSQLの特徴のひとつである分散アーキテクチャーを持つサーバーソフトウェアと、組み込みデータベースという根本の違いがあるので、言葉通りに受け取るのは危険ですが」

「そうか、それはそうですよね」

「当然、実装上コードを共有しているわけでもありません。言うなれば、どちらともドキュメント指向であることに始まり、多くの機能上の共通点を持つファミリーであるとは言えるかな」

「ドキュメント指向という他に、どんな特徴がありますか？」

「Couchbase ServerとCouchbase Liteに共通した特徴としては、SQLを使えることがあります」

「え、何でここでSQLの話が出てくるんですか?NoSQLですよね」

「厳密に言えば、SQLをJSONデータへのクエリのために拡張した言語です。SQL++とも呼ばれます」

「SELECT…FROMみたいなクエリを使うんですか?」

「クエリ文字列を使う方法と、クエリビルダーAPIという、クラスを使って式を構築する方法の二通りがあります。誤解のないようにいうと、ここからはCouchbase Liteにフォーカスしてお話ししますね。
Couchbase Liteでタイプセーフなコードを優先する場合は、クエリビルダーAPIを使うことができます。
もともと、クエリビルダーAPIのみが提供されていたところ、クエリ文字列を使う方法は後から追加されたという背景があります。Couchbase Liteは、C言語でも使うことができるのですが、C APIではクエリ文字列を使う方法のみが提供されています。C言語がサポートされたバージョンで、他のプログラミング言語でもクエリ文字列を使う方法が提供された、という経緯があります」

「C言語での開発でもCouchbase Liteが使えるんですね」

「はい。Couchbase Liteは、もともとAndroid Javaだけでなく、通常のJavaにも対応していたので、スマートフォン向けのモバイルアプリに限らず、JVMが利用できる環境で使うことができました。加えて、C言語に対応したことによって、より幅広い実行環境で、かつJavaを利用するよりも軽量なフットプリントで使うことができるようになっています」

「なるほど、エッジデバイス向けデータベースの選択肢にもなるということですね」

「そうだね。エッジでのユースケースとして、たとえばセンサーデータを収集するアプリケーションを考えてみると、センサーの扱うデータ項目が変更された場合にも、ドキュメント指向データベースであれば対応が容易という面があります」

「なるほど、スキーマレスの利点は、そういう点に見つけることができるんですね」

「さらに、データセンターやクラウドとのデータ同期についても、Couchbaseに委ねることができます」

「RealmとMongoDBの組み合わせのようなことでしょうか?もう少し、説明してもらってもいいですか？」

「Couchbase Liteは、単体の組み込みデータベースとして使うことももちろんできますが、Couchbase Serverとの双方向のデータ同期を行うための機能を内蔵しています」

「元々、連携するように設計されているということですね」

「その通り。これによって、クライアントとサーバーとのデータのやり取りという典型的かつ定型的な処理をデータプラットフォームに任せることができます。つまり、プロジェクトは本来のビジネスのための開発にフォーカスすることができる、ということですね」

「MBaaSのニーズともクロスする部分ですね」

「さらにいえば、モバイルアプリとWebアプリのようなマルチチャネルで共通のサービスを展開するニーズにもつながってきます」

「なるほど。押さえておくべきことがわかってきました。自分でも調べてみますが、何か気をつけておくべきことなどありますか?」

「クラウドやデータセンターとの同期やMBaaSまで視野に入れているのであれば、サーバーサイドのデータベースについてもそれなりに把握しておくことが重要になるかと思います。たとえば、ドキュメント指向データベースについては、MongoDB、Couchbaseだけでなくクラウドプロバイダーが提供しているものもありますし、それぞれで格納できるドキュメントのサイズ等、仕様も異なっています。この辺りは、まだ単純なところですが」

「はい、調べてみます」

「よりシビアなところでいうと、性能面が焦点になるかと思います。一口にモバイルないしエッジと言っても、業務用専用端末や工場に置かれたデバイスのようなインハウスのシステムなのか、それともコンシューマー向けの展開なのか、といったサービスの性質にもよりますが、拡張性との関係も考慮する必要があります。おそらく元々想定されていた調査の範囲を超えているでしょうが、意識しておく価値はあるはずです」

「確かに。でも、どこから手を付ければいいのか…」

「私が知っている範囲で、NoSQLの性能比較について、三つのリサーチペーパーが公開されています。ひとつは、Couchbase ServerとMongoDB、そしてDataStax Enterpriseを比較したもので、環境としてはAWSがIaaSとして使われています。@<fn>{nosql-performance-benchmark}もうひとつは、DBaaSとしてのNoSQLを比較したもので、Couchbase ServerのDBaaSであるCouchbase Capellaと、MongoDBのDBaaSであるMongoDB Atlas、そしてAmazon DynamoDBとを比較しています。@<fn>{nosql-dbaas-performance-benchmark}最後は、やはりDBaaS同士を比較したもので、Couchbase CapellaとAzure Cosmos DBとを比較しています。@<fn>{nosql-dbaas-performance-benchmark-azure}後でそれぞれのURLを送っておきます」

「ありがとうございます!」

「ひとまず、こんなところでしょうか?」

「はい。後は、こちらで整理してみます」

「お伝えしたことが、プロジェクトの適性に応じたデータベース選択の参考になれば嬉しいです」

//footnote[nosql-performance-benchmark][https://www.altoros.com/research-papers/performance-evaluation-of-nosql-databases-with-ycsb-couchbase-server-datastax-enterprise-cassandra-and-mongodb/]

//footnote[nosql-dbaas-performance-benchmark][https://www.altoros.com/research-papers/performance-evaluation-of-nosql-databases-as-a-service-couchbase-capella-mongodb-atlas-amazon-dynamodb/]
//footnote[nosql-dbaas-performance-benchmark-azure][https://www.altoros.com/research-papers/performance-evaluation-of-nosql-databases-as-a-service-2021-couchbase-capella-and-azure-cosmosdb/]