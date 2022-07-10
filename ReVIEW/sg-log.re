= Sync Gatewayロギング

Sync Gatewayのロギング(Logging@<fn>{sg-logging})について解説します。
Sync Gatewayのログ出力先には、ファイルとコンソールのふたつがあります。

//footnote[sg-logging][https://docs.couchbase.com/sync-gateway/current/logging.html]


== ログファイル

ファイルベースのロギングでは、ログは、ログレベルでフィルタリングされた個別のログファイルに書き込まれます。

=== ログファイルの種類

ログファイルの種類には次のものがあります。: @<tt>{ERROR}, @<tt>{WARN}, @<tt>{INFO}, @<tt>{DEBUG}

//blankline

@<tt>{DEBUG}ログはデフォルトで無効になっています。

=== ファイルローテーション

@<tt>{logging.console.rotation}プロパティを使用してログのローテーションを構成します。

ログファイルは、@<tt>{max_size}に指定したサイズを超えるとローテーションされます。ローテーションされたファイルは、ディスク使用量を減らすために圧縮されます。

ログの更新日付が@<tt>{max_age}に指定した日数を超えると、そのログはクリーンアップされます。
@<tt>{max_age}のデフォルト値と最小値は、ログの種類(@<tt>{ERROR}, @<tt>{WARN}, @<tt>{INFO}, @<tt>{DEBUG})によって異なります。


== コンソールログ

=== 利点

コンソールログ設定は、ファイルへのログ出力とは独立しているため、基本的なログを損なうことなく、次の点を調整できます。

//blankline

 * ログレベルを使用して詳細度を上げ、追加の診断情報を生成します
 * 特定のログキーを有効または無効にして、調査中の領域に焦点を合わせます
 * ログレベルに基づいてログ出力の色を設定することにより、読みやすさを向上させることもできます。

=== ログレベル

コンソールログに出力する情報の重要度を制御するために、ログレベルが提供されています。

ログレベルには、次の6つのレベルがあります。: @<tt>{none}, @<tt>{error}, @<tt>{warn}, @<tt>{info}, @<tt>{debug}, @<tt>{trace}


=== ログキー

コンソールログに出力する情報の種類を制御するために、ログキーが提供されています。
デフォルトでは、HTTP関連情報のみが有効になっていますが、特定の診断ニーズを満たすために、他のさまざまなキーを使用できます。

ログキーには、次の種類があります。: @<tt>{*}, @<tt>{none}, @<tt>{Admin}, @<tt>{Access}, @<tt>{Auth}, @<tt>{Bucket}, @<tt>{Cache}, @<tt>{Changes}, @<tt>{CRUD}, @<tt>{DCP}, @<tt>{Events}, @<tt>{gocb}, @<tt>{HTTP}, @<tt>{HTTP+}, @<tt>{Import}, @<tt>{Javascript}, @<tt>{Migrate}, @<tt>{Query}, @<tt>{Replicate}, @<tt>{SGCluster}, @<tt>{Sync}, @<tt>{SyncMsg}, @<tt>{WS}, @<tt>{WSFrame}

//blankline

各ログキーの意味については、ドキュメント(Log Keys@<fn>{lbl-log-keys})を参照ください。

//footnote[lbl-log-keys][https://docs.couchbase.com/sync-gateway/current/logging.html#lbl-log-keys]

== ロギング構成方法

Sync Gatewayでは、ブートストラップ構成や、管理REST APIを使ってロギングを構成します。


=== ブートストラップ構成

ロギングは、ノードレベルのプロパティとして、ブートストラップ構成にて管理されます。

以下は、ログに関する構成箇所の例です。

//emlist[][json]{
{
  "logging": {
    "log_file_path": "/var/tmp/sglogs", 
    "redaction_level": "partial", 
    "console": { 
      "log_level": "debug",
      "log_keys": ["*"]
      }, 
    "error": { 
      "enabled": true,
      "rotation": {
        "max_size": 20,
        "max_age": 180
        }
      },
    "warn": { 
      "enabled": true,
      "rotation": {
        "max_size": 20,
        "max_age": 90
        }
      },
    "info": { 
      "enabled": false
    },
    "debug": { 
      "enabled": false
    }
  }
}
//}

ブートストラップ構成ファイルを作成する際には、Sync Gatewayインストール時に提供される各種サンプル構成ファイルを参照することができます。

ブートストラップ構成ファイルのプロパティの詳細については、ドキュメント(Bootstrap Configuration@<fn>{configuration-schema-bootstrap})を参照ください。


=== 管理REST API

ロギング設定を、管理REST APIを使って動的に変更することも可能です。
ただし、管理REST APIによるロギングプロパティへの変更は再起動後は引き継がれません。

詳細はドキュメント@<fn>{lbl-log-api}を参照ください。

//footnote[configuration-schema-bootstrap][https://docs.couchbase.com/sync-gateway/current/configuration-schema-bootstrap.html]

//footnote[lbl-log-api][https://docs.couchbase.com/sync-gateway/current/logging.html#lbl-log-api]