= プロローグ: データベース選択を巡る対話

ここはある技術系ミートアップ会場、有志によるLT(ライトニングトーク)が行われた後のフリータイムに、各所で思い思いの会話がなされています。

その一角で、また新しい対話が始まります。

//blankline

「先ほど発表された内容、とても勉強になりました」

「ありがとうございます。あなたの新しいアプリケーションの構想も、なかなか興味深かったですよ」

「そうですか！まだ、アイディアでしかないんですけどね。ところで、少し相談させてもらってもいいですか？」

「自分でよければ」

「ありがとうございます！かなり漠然とした相談になってしまうのですが、先ほど発表したアプリケーションのプロトタイプを開発するに当たって、どのデータベースを使うか悩んでいます。データベースについて、広い知見をお持ちのようでしたので...」

「広いかどうかは分からないけど。どんな部分で悩んでいますか？右も左も分からないという感じではなさそうだけど」

「あまり深く考えずに、とりあえず作り始めるとしたら、何か適当なRDBを使うと思うんですよね...」

「はい」

「プロトタイプを作っている間、データモデルについては試行錯誤すると思うんです。それだけなら、テーブル定義を修正しながら進めてもいいんですが...」

「ふんふん」

「プロトタイプとは言っても、可読性と保守性の高いコードを維持したいんです。これは絶対に！」

「いいですね」

「だから、ドメインエンティティを素直に反映したデータモデルでコーディングを進めたくて、RDBでデータを扱う際の制約とは切り離しておきたいんです」

「分かります」

「それで、ORM(オブジェクトリレーショナルマッピング)について考え始めると、特定の技術への偏りと、そのための負担が、煩わしくなってくるんですよね」

「そういうことですね。今の話を聞くと、すでに何かRDB以外で検討していそうだけど、どうですか？」

「はい。ドキュメント指向データベースというんですか？JSONを保存できるデータベースなら、アプリケーションで扱っているエンティティと同じ構造のデータを保存できると思ったんですが...」

「そうですよね。だけど？」

「それはそれで、実際に開発を始めようとすると、また別の技術的な偏向というか、負担が気になるんですよね...」

「ドキュメント指向データベースは、開発の面について、SQLのように標準化されているわけではないですからね」

「そうなんです！仕方がないんですかね？」

「それでは、まず確認ですが、クラウド利用については、どう考えていますか？」

「もちろん、いずれクラウドは有効に活用したいと思っていますが、今は、特定のクラウドベンダーに縛られたくはなくて。開発中は、ラップトップで完結させたいと思っています」

「それでは、クラウドベンダー独自の技術は自ずと除外されるとして...」

「何か思い当たりますか？」

「Couchbaseって、聞いたことあるかな？」

「う〜ん、名前だけは聞いたことがあるような...Apacheプロジェクトでしたっけ？」

「いや、Apache CouchDBというのはあるけど、それとは別のオープンソースのデータベースです」

「知ったかぶりして恥ずかしい！」

「いやいや。多分、名前はどこかで聞いていたんでしょうね」

「あらためて、何も知らないものとして、教えてもらえますか？」

「それでは、お話を伺って、興味を持たれるかもしれないと思ったCouchbaseの特徴について挙げていきますね」

「お願いします！」

「まずは、JSONドキュメントを扱うドキュメント指向データベースの中でも、多くのプラットフォームで稼働するオープンソースのテクノロジーであるということ」

「それは、私の希望しているところでもあります」

「それから、開発の面で、SQLを使えるところ...」

「そこを詳しく教えてもらえますか！いろいろなRDB以外のデータベースがクエリを使えると謳っていて、少し調べると、なんだか...」

「期待していたものと違う」

「そうなんです。よく理解できていないだけなのかもしれませんが...逆に、RDBがJSONに対応した、というケースもありますよね。どちらも、なんだか、しっくりこないんです」

「SQLをJSON用に拡張する時の1つの方法として、JSONを扱うための関数を追加する、というものがあります。この場合、純粋なSQLのシンタックスは保持されますが、JSONデータから発想している開発者にとっては、期待しているものとのギャップがあるのではないかと思います」

「はい」

「一方で、関数の追加ではなく、SQLのシンタックス自体をJSONデータ構造用に拡張するというアプローチがあります」

「全然違いますね」

「JSONの特徴として、配列やネストされたデータが扱えるということがありますよね」

「はい、分かります」

「例えば、SQLのクエリで直接、配列の表現が使えたり」

「ははあ」

「オブジェクトのプロパティにドット表記でアクセスできたり、普段から開発者がプログラミングで用いている表現が使えれば直感的ですよね」

「そうですね」

「先ほど、期待していたものと違う、という話がありましたが、何が一番、ギャップとして大きいと感じますか？」

「当然の質問だと思うんですが、正直、よく説明できないんですよね...」

「意地悪な質問だったかもしれませんね。今、話したようなデータ構造の違いだけなら、関数でも吸収できるでしょう。
だけど、データ単体ではなく、複数のデータを扱うことを考え始めると、話が違ってきます。
端的にいえば、データを結合できるかどうかが問題になると思うんです」

「何となく分かるような気がします」

「JSONが、ネストした構造を持てるからと言って、関係する全てのエンティティを1つのドキュメントとして保存するのは現実的ではありませんよね」

「確かに。できることと、やるべきことの違いですね」

「参照キーでデータ間の関係を表現するのは、リレーショナルモデルの中心にある発想ですが、データモデルの設計としては、RDBのテーブルに限らず、他のデータ構造でも適用可能な考え方です」

「それはそうですね」

「後は、複数のデータを結合してデータを取り出せるかどうか？という話になってきます」

「ふむふむ」

「その点、CouchbaseのN1QL(ニッケル)は、期待されるものを実現しています」

「ニッケル、ですか？」

「Couchbaseのクエリの呼称です。Non 1st normal form Query Languageの略で、N1QLと書いて、ニッケルと読みます」

「なるほど。JSONは、RDBのように第一正規化が必要ないから、非第一正規形クエリ言語、というわけですね」

「そうそう。今、第一正規化が必要ない、と言われましたよね。必要ないだけで、逆に第一正規化されている、つまり普通の表形式のデータであっても何の問題もない、というのがポイントです」

「というと？」

「例えば、既存のリレーショナルデータベースで扱っているデータを、データモデルを変えずに、JSONデータとしてCouchbaseに移植した場合には、RDBで使っていたのと全く同じSQLを使う、といったことも可能になってきます」

「へえ。なんとなく、技術的に偏った方向に振れすぎる、という心配を解消してくれそうな気がしてきました」

「うん、極端な話、Couchbaseでプロトタイプを作った後、RDBに移行するということも、設計次第で成立させられると思いますね。メリットがあるかどうかは別として」

「面白いですね。実際にはありそうではなくても、そういうことも考えられるくらい汎用性があって、標準的な知識を活かすことができる、というのは魅力的だと思います」

「標準という意味では、SQL++@<fn>{arxiv-sqlpp}という準構造化データへのクエリに関する標準規格があって、ApacheプロジェクトにもAsterixDB@<fn>{asterixdb-apache-org}というSQL++を採用しているデータベースがあります。CouchbaseのN1QLは、SQL++と全く同じものではないけど、多くの共通する部分を持っています@<fn>{n1ql-explanation}」



//footnote[arxiv-sqlpp][https://arxiv.org/abs/1405.3631]

//footnote[asterixdb-apache-org][https://asterixdb.apache.org/]

//footnote[n1ql-explanation][上に紹介した論文でもN1QLに触れられており、「syntactic sugar over SQL++」という表現が見られます。]

「それは、悪くない情報ですね」

「ご相談に直接関係する部分としては、こんなところですかね。お役に立てたでしょうか？」

「はい、ありがとうございます」

「あと、Couchbaseを使ったNode.jsのサンプルアプリケーションを持っていますが、見てみますか？」

「ぜひ！」

「Couchbaseとアプリケーションを起動しますね...画面はこんな感じです。ご覧の通り、とても単純なユーザー管理アプリケーションです」

「これくらい簡潔だと、内部で行われていることを理解しやすそうですね」

「これがソースです」

「ユーザーのリスト表示に使っているクエリがこれですね。確かに普通のSQLと同じですね」

「他には、ユーザーの追加と削除しか機能がありませんが、それはこの辺りです」

「INSERTやDELETEのクエリを使うわけではないんですね」

「使うこともできるけどね。データを直接操作して済むなら、わざわざ文字列でクエリを渡して、サーバーでそれを解析してから実行して...そんな必要はないということですね」

「ORMを使っている時と似ているかも」

「こういう部分、ORMではSQLを隠しますよね。ここではそういったラッパーを使っていないので、リソース消費の上でも、知識習得の面でも、余計なオーバヘッドを避けることができる、と言えるんじゃないかな」

「なるほど」

「反対に、データを検索する時には、SQLの方が開発者にとって直感的なのに、ORMを使うために、余計な苦労を強いられることもあるんじゃないですか？」

「分かる気がします。私がRDBとORMの組み合わせを前提に開発を進めることに二の足を踏んでいたのも、そんなところです」

「ここまでのところ、どうですか？」

「プログラムを見せていただいたので、具体的にイメージが掴むことができました。コード量もこれだけで、他にフレームワークやライブラリは使っていないんですよね？」

「はい。データベースアクセスに関して、Couchbaseライブラリのみを使っています。はじめにクラスターへの接続を確立しておいて、必要なところで、データ表示のためのN1QLクエリと、データ操作のためのAPIコールを行っています」

「ここで使っているのは、とても基本的なクエリですが、自分でもっと複雑なことをする場合も、SQLが使えるから応用が効きそうです」

「役に立ちそうですか？」

「はい！何より、シンプルなのが気に入りました」

「それはよかった。そうそう、今見てもらったように、開発環境をラップトップで完結させたい、という要望にも適っていると思います」

「ちなみに、本格的な運用では、どうなりますか？」

「NoSQLという言葉は聞かれたことがあると思いますが、Couchbaseは、NoSQLにカテゴライズされるデータベースです。他のNoSQLデータベースと同じように、複数のノードからなるクラスターとして構成されます」

「ああ、それ！そのクラスターというところなんですが...やっぱり構築とか大変ですよね？」

「そんなことありませんよ。Couchbaseの良いところは、他の分散データベースのように、マスターとワーカーとか、レプリカセットのような、特別な構成が必要ない事です。全てのサーバーに同じものをインストールします」

「すみません、構築が他のNoSQLより簡単ということですが、今一つ、どこが違うかが分からなくて...」

「データベースを複数台で構成する利点として、データを複製して持つことと、データを分散して持つことがあります。この違いについては、分かりますか？」

「データが複製されているから、１つが失われても大丈夫だし、データが分散されているから、処理も分散できる、ということですよね」

「そうですね。そして、多くのNoSQLでは、データを分散する方法が、データのキーの設計に関係してきます」

「キーをどのように決めるかによって、複数のサーバーへのデータの散らばり方が変わってくるということですね。NoSQLでは、どんな検索条件が使われることになるかを踏まえてデータベースを設計する必要がある、という話を聞いた覚えがあります」

「Couchbaseでは、物理的なデータの分散方法とキーの設計が独立しているのが特徴です。つまり、キーの設計による性能への影響がない、ということですね。これは、データの物理配置に影響を受けないアーキテクチャーになっているからですが、それだけでなく、他のデータベースと比べて、性能面で優位なアーキテクチャーで...という話は、長い話になってしまうので置いておくとして...データ設計を行うアプリケーション開発者にとっては、分散環境の構成に関する知識を前提とする必要がない、という利点があります」

「アーキテクチャーについての話も興味はありますが、まずはプロトタイプ開発なので、またそのうちに...データの論理設計と物理設計が切り離されているのはいいですね！」

「それで、どこが違うかが分からないという話に戻ると、確かに本番環境の構築となると神経を使うところはあると思いますが、Couchbaseの場合、開発用に、1ノードで構成されたクラスターを使っている時と、本番環境で、複数ノードのクラスターを構築した時とで、論理的な構成が変わらない、お言葉を借りるなら、シンプルな、システム構成になっています。これは、開発される立場から見てもそうであるように、構築する際にも、利点となります」

「なるほど、そういうことですね」

「他に気になるところ、ありますか？」

「今は大丈夫です。簡単に使い始められそうなので、まずは触ってみたいと思います」

「そうですか。それでは、私からもいいですか？」

「何でしょう？」

「LTで発表されていたアプリケーションについて、聞いてみたいことがあるんですが...」

「何でも聞いてください！」

//blankline

それぞれの関心を交差させながら、会話はさらに続くようです...