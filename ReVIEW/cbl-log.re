
= Couchbase Liteロギング

Couchbase Liteのトラブルシューティングのためのログ利用(Using Logs for Troubleshooting@<fn>{troubleshooting-logs})について解説します。


//footnote[troubleshooting-logs][https://docs.couchbase.com/couchbase-lite/current/android/troubleshooting-logs.html]

== 概要

Couchbase Liteのログ出力先には、ファイルとコンソールのふたつがあります。
これらのログ出力は、ログドメインとログレベルによって制御されます。

=== ログドメイン

ログドメインには、次の種類があります。

//blankline

@<tt>{DATABASE}, @<tt>{LISTENER}, @<tt>{NETWORK}, @<tt>{QUERY}, @<tt>{REPLICATOR}

=== ログレベル

ログレベルには、次の種類があります。

//blankline

@<tt>{DEBUG}, @<tt>{ERROR}, @<tt>{INFO}, @<tt>{NONE}, @<tt>{VERBOSE}, @<tt>{WARNING}


=== ログファイル

ファイルベースのロギングでは、ログはログレベルでフィルタリングされた個別のログファイルに書き込まれます。
ファイルベースのログはデフォルトで無効になっています。

ファイルベースのログ出力フォーマットには、バイナリと平文(プレインテキスト)の２通りがあります。

バイナリフォーマットは、ストレージとパフォーマンスに最も効率的であり、ファイルベースログ出力のデフォルトフォーマットです。

ファイルベースのロギングでは、@<tt>{LogFileConfiguration}クラスのプロパティを使用して以下を指定することができます。

//blankline

 * ログファイルを保存するディレクトリー
 * 出力フォーマット: バイナリまたは、プレインテキスト。デフォルトはバイナリです。必要に応じて、プレーンテキストのログに変更できます。
 * ログファイルの最大サイズ（バイト）: この制限を超えるとログファイルのローテーションが行われます。
 * ローテーションされたログファイルを保持する最大数: デフォルトは「1」です。例えば、この設定を「5」にした場合、ローテーションされた5つのログファイルとアクティブなログファイルの最大6ファイルが存在することになります。

//blankline

以下は、上記設定の適用例です。

//emlist[][JAVA]{
final File path = context.getCacheDir();

// ログファイルディレクトリを設定
LogFileConfiguration LogCfg = new LogFileConfiguration(path.toString());

// ログファイルの最大サイズ（バイト）を設定
LogCfg.setMaxSize(10240);

// 最大回転数を5に変更
LogCfg.setMaxRotateCount(5);

// フォーマットをプレインテキストに変更
LogCfg.setUsePlaintext(true);

Database.log.getFile().setConfig(LogCfg);

// ログ出力レベルをデフォルト(WARN)から「INFO」に変更
Database.log.getFile().setLevel(LogLevel.INFO);
//}

=== コンソールログ

コンソールベースのロギングでは、ログはコンソールに出力されます。
コンソールログはデフォルトで有効になっています。

コンソールログはファイルログとは独立に設定を行うことができるため、通常のファイルベースのログに干渉することなく、問題調査のための診断シナリオに合わせて出力情報を調整できます。

デフォルトのログでは情報が不十分な場合に、下記のようにデータベースエラーに焦点を当てて、より詳細なメッセージを得ることができます。

//emlist[][java]{
Database.log.getConsole().setDomains(LogDomain.DATABASE);
Database.log.getConsole().setLevel(LogLevel.VERBOSE); 
//}

== カスタムロギング

コールバック関数を登録してログメッセージを受信することができます。
受信したメッセージは、何らかの外部のログフレームワークを使用してログに記録できます。

カスタムロギングを行うには、@<tt>{com.couchbase.lite.Logger}インターフェースを実装します。

ロガーインターフェイスを実装するコードを紹介します。

//emlist[][JAVA]{
class LogTestLogger implements Logger {

    private final LogLevel level;

    public LogTestLogger(LogLevel level) { this.level = level; }

    @Override
    public LogLevel getLevel() { return level; }

    @Override
    public void log(LogLevel level, LogDomain domain, String message) {
	    //カスタムロギング処理の実装
    }
}
//}

@<tt>{Logger}インターフェースの実装に加えて、@<tt>{Database}に対して、カスタムロギングを有効にする必要があります。
以下の例では、「警告」のレベルに設定して、カスタムロガーを追加しています。

//emlist[][JAVA]{
Database.log.setCustom(new LogTestLogger(LogLevel.WARNING)); 
//}


== バイナリログのデコード

バイナリログファイルは、Couchbase Liteのコマンドラインツール@<tt>{cblite}を使って、デコードすることができます。 
@<tt>{cblite}の利用方法について、次の章で説明しています。

また、ログ専用のコマンドラインツール@<tt>{cbl-log}も提供されています。
@<tt>{cbl-log}ツール(The cbl-log Tool@<fn>{cbl-log})は、Couchbase LabsのGitHubリポジトリThe Couchbase Mobile Tool Repo@<fn>{couchbase-mobile-tools}にて公開されています。

@<tt>{cbl-log}の利用法については、ドキュメント@<fn>{decoding-binary-logs}を参照ください。
また、ライセンス、利用規約や詳細については、リポジトリの記載をご確認ください。

//footnote[couchbase-mobile-tools][https://github.com/couchbaselabs/couchbase-mobile-tools/]

//footnote[cbl-log][https://github.com/couchbaselabs/couchbase-mobile-tools/blob/master/README.cbl-log.md]

//footnote[decoding-binary-logs][https://docs.couchbase.com/couchbase-lite/current/android/troubleshooting-logs.html#decoding-binary-logs]

