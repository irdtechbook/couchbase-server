
= SDKと外部システム連携

== SDK

=== サポートされるプログラミング言語

アプリケーションからCouchbaseクラスターへアクセスするため、各種プログラミング言語のSDKが提供されています。

以下のプログラミング言語のSDKが、公式サポートされています。

 * Java@<fn>{java-sdk}
 * Scala@<fn>{scala-sdk}
 * .Net@<fn>{dotnet-sdk}
 * C@<fn>{c-sdk}
 * Node.js@<fn>{nodejs-sdk}
 * PHP@<fn>{php-sdk}
 * Python@<fn>{python-sdk}
 * Ruby@<fn>{ruby-sdk}
 * Go@<fn>{go-sdk}

 また、デベロッパープレビューとして、Kotlin SDK@<fn>{kotlin-sdk}があります。

 さらに、コミュニティサポートとしてRust SDK@<fn>{couchbase-rs}があります。

//footnote[java-sdk][https://docs.couchbase.com/java-sdk/current/hello-world/overview.html]
//footnote[scala-sdk][https://docs.couchbase.com/scala-sdk/current/hello-world/overview.html]
//footnote[dotnet-sdk][https://docs.couchbase.com/dotnet-sdk/current/hello-world/overview.html]
//footnote[c-sdk][https://docs.couchbase.com/c-sdk/current/hello-world/overview.html]
//footnote[nodejs-sdk][https://docs.couchbase.com/nodejs-sdk/current/hello-world/overview.html]
//footnote[php-sdk][https://docs.couchbase.com/php-sdk/current/hello-world/overview.html]
//footnote[python-sdk][https://docs.couchbase.com/python-sdk/current/hello-world/overview.html]
//footnote[ruby-sdk][https://docs.couchbase.com/ruby-sdk/current/hello-world/overview.html]
//footnote[go-sdk][https://docs.couchbase.com/go-sdk/current/hello-world/overview.html]

//footnote[kotlin-sdk][https://docs.couchbase.com/kotlin-sdk/current/hello-world/overview.html]

//footnote[couchbase-rs][https://github.com/couchbaselabs/couchbase-rs]

=== 拡張ライブラリ

分散ACIDトランザクションのためのライブラリが、以下の言語向けに提供されています。

 * Java@<fn>{java-sdk-distributed-acid-transactions}
 * C#@<fn>{dotnet-sdk-distributed-acid-transactions}
 * C++@<fn>{cxx-txns}

//footnote[java-sdk-distributed-acid-transactions][https://docs.couchbase.com/java-sdk/current/howtos/distributed-acid-transactions-from-the-sdk.html]
//footnote[dotnet-sdk-distributed-acid-transactions][https://docs.couchbase.com/dotnet-sdk/current/howtos/distributed-acid-transactions-from-the-sdk.html]
//footnote[cxx-txns][https://docs.couchbase.com/cxx-txns/current/distributed-acid-transactions-from-the-sdk.html]
 
== 外部システム連携

=== コネクター

Couchbase Serverと外部システムを連携するため、以下のコネクターが提供されています。

 * Elasticsearch
 * Kafka
 * Spark

詳細は、ドキュメント(Connector Guides@<fn>{connectors_intro})をご参照ください。

//footnote[connectors_intro][https://docs.couchbase.com/server/current/connectors/intro.html]

