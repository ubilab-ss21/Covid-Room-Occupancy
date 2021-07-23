import 'package:flutter/material.dart';
import 'package:flutter_mqtt_app/covidappview/MQTTCovidAppView.dart';
import 'package:flutter_mqtt_app/covidappclient/MQTTCovidAppState.dart';
import 'package:provider/provider.dart';

void main() => runApp(MyApp());

class MyApp extends StatelessWidget {
  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    
    return MaterialApp(
        title: 'Flutter Demo',
        theme: ThemeData(
        primarySwatch: Colors.blue,
        ),
        home: ChangeNotifierProvider<MQTTCovidAppState>(
          create: (_) => MQTTCovidAppState(),
          child: MQTTCovidAppView(),
        ));
  }
}
