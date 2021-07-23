import 'dart:convert';
import 'dart:io' show Platform;
import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:flutter_mqtt_app/covidappclient/MQTTCovidAppState.dart';
import 'package:flutter_mqtt_app/covidappclient/CovidAppManager.dart';
import 'package:syncfusion_flutter_gauges/gauges.dart';
import 'package:flutter_vibrate/flutter_vibrate.dart';
import 'package:flutter_ringtone_player/flutter_ringtone_player.dart';
import 'package:rflutter_alert/rflutter_alert.dart';

class MQTTCovidAppView extends StatefulWidget {
  @override
  State<StatefulWidget> createState() {
    return myViewState();
  }
}

class myViewState extends State<MQTTCovidAppView> {

  late MQTTCovidAppState currentAppState;
  late CovidAppManager manager;

  // local variable
  int mynumber = 0;
  String numberofPeople = "";

  int uvone = 0;
  int uvtwo = 0;
  double ppm = 0;
  double humidity = 0;
  double temperature = 0;

  final List<double> sensordata = List.filled(10, 0);
  Map<String, dynamic> map = <String, dynamic>{};

  // call the parent class init state
  @override
  void initState() {
    super.initState();
  }

  // call the parent class dispose function
  @override
  void dispose() {
    super.dispose();
  }

  // this function builds the GUI and response to events
  @override
  Widget build(BuildContext context) {
    final MQTTCovidAppState appState = Provider.of<MQTTCovidAppState>(context);

    currentAppState = appState;
    final Scaffold scaffold = Scaffold(
        appBar: AppBar(
            title: Text('COVID ROOM OCCUPANCY'),
            centerTitle: true,
            backgroundColor: Colors.blueGrey[600]),
        body: buildCovidAppHelper(context));
    return scaffold;
  }

  // this function shows the screen
  Widget buildCovidAppHelper(BuildContext context) {
    return Column(
      children: <Widget>[
        buildGUI(currentAppState.getReceivedText,context),

        buildButtons(),
        const SizedBox(height: 60),
        buildConnectionText(
            checkConnection(currentAppState.getAppConnectionState))
      ],
    );
  }

  // this function shows the buttons
  Widget buildButtons() {
    return Padding(
      padding: const EdgeInsets.all(20.0),
      child: Column(
        children: <Widget>[
          const SizedBox(height: 10),
          _buildConnecteButtonFrom(currentAppState.getAppConnectionState)
        ],
      ),
    );
  }

  // this function shows the connections status
  Widget buildConnectionText(String status) {
    return Row(
      crossAxisAlignment: CrossAxisAlignment.center,
      verticalDirection: VerticalDirection.up,

      children: <Widget>[
        Expanded(
          child: Container(
              padding: EdgeInsets.all(15),
              color: Colors.lightGreenAccent,
              child: Text(status, textAlign: TextAlign.center)),
        ),
      ],
    );
  }

  // this function shows the count as RadialGauge and Ultrasonic
  Widget buildGUI(String text,BuildContext context) {
   // try to decode the json sent over mqtt
    try {
      //get the count
      map = jsonDecode(text);
      mynumber = map["values"];
      numberofPeople = mynumber.toString();

      //get the uv sensor value
      uvone = map["uvone"];
      uvtwo = map["uvtwo"];
      temperature = map["temperature"];
      humidity = map["humidity"];
      ppm = map["ppm"];

      sensordata.add(uvone.toDouble());
      // catch exception
    } catch (_) {
      print("mqtt json textstring exception - 182");
    }

    // if the number has exceeded threshold , enable the warnings
    if (mynumber > 5) {
      Vibrate.feedback(FeedbackType.warning);
      FlutterRingtonePlayer.playAlarm();
      //player.play('buzzerSound.mp4');
    }

    if(mynumber <= 5){
      FlutterRingtonePlayer.stop();

    }
    // GUI of radial Gauge and Linear Gauge
    return Column(children: <Widget>[
      //Initialize the chart widget
      Card(
          margin: EdgeInsets.all(10),
          elevation: 10,
          child: SfRadialGauge(
            title: GaugeTitle(
                text: 'Number of people in the room', textStyle: TextStyle(
                fontSize: 20.0, fontWeight: FontWeight.bold)),
            axes: <RadialAxis>[
              RadialAxis(minimum: 0,
                  maximum: 15,
                  ranges: <GaugeRange>[
                    GaugeRange(startValue: 0,
                        endValue: 5,
                        color: Colors.green,
                        startWidth: 10,
                        endWidth: 10),
                    GaugeRange(startValue: 5,
                        endValue: 10,
                        color: Colors.orange,
                        startWidth: 10,
                        endWidth: 10),
                    GaugeRange(startValue: 10,
                        endValue: 15,
                        color: Colors.red,
                        startWidth: 10,
                        endWidth: 10)
                  ],
                  pointers: <GaugePointer>[
                    NeedlePointer(value: mynumber.toDouble())
                  ],
                  annotations: <GaugeAnnotation>[
                    GaugeAnnotation(widget: Container(child:
                    Text(numberofPeople, style: TextStyle(
                        fontSize: 25, fontWeight: FontWeight.bold))),
                        angle: 90, positionFactor: 0.5)
                  ]
              )
            ],
          )),
      const SizedBox(height: 10),
      Card(
        margin: EdgeInsets.all(10),
        elevation: 10,
        child: Column(
            children: <Widget>[
              const ListTile(
                leading: Icon(Icons.sensors),
                title: Text('HC-SR04 Data'),
                subtitle: Text('Distance in cm'),
              ),
              SfLinearGauge(
                  minimum: -10.0,
                  maximum: 150.0,
                  barPointers: [
                    LinearBarPointer(
                        value: uvone.toDouble(),
                        animationDuration: 2000,
                        animationType: LinearAnimationType.bounceOut,
                        position: LinearElementPosition.outside
                    ),
                    LinearBarPointer(
                        value: uvtwo.toDouble(),
                
                        offset: 10,
                        animationDuration: 2000,
                        animationType: LinearAnimationType.bounceOut,
                        position: LinearElementPosition.outside
                    ),
                  ]),
            ]),
      ),
             Card(
            child: Row(
    children: <Widget>[
      SizedBox(width:40),
    new Text("Temp:" + temperature.toInt().toString(),
    style:TextStyle(
        color:temperature > 1000 ?  Colors.red : Colors.green,
        fontWeight: FontWeight.bold,
        fontSize: 15)),
      SizedBox(width:40),
    new Text("CO2:" + ppm.toInt().toString(),
        style:TextStyle(
            color:ppm > 1000 ?  Colors.red : Colors.green,
            fontWeight: FontWeight.bold,
            fontSize: 15)),
      SizedBox(width:30),
    new Text("Humidity:" + humidity.toInt().toString(),
        style:TextStyle(
            color: Colors.green,
            fontWeight: FontWeight.bold,
            fontSize: 15)),
      SizedBox(width:40),
    ],
    ),
    ),
    ]);
  }

  // connect to uni broker on press of button
  Widget _buildConnecteButtonFrom(MQTTCovidAppConnectionState state) {
    return Row(
      children: <Widget>[
        Expanded(
       
          child: RaisedButton(
            padding: EdgeInsets.all(10),
            color: Colors.blueGrey,
            child: Row(
                children: <Widget>[
                  Icon(Icons.network_wifi),
                  const Text('MQTTConnect')
                ]),
            onPressed: state == MQTTCovidAppConnectionState.disconnected
                ? CovidAppInit
               : null, //
          ),
        ),
        const SizedBox(width: 10),
        Expanded(
       
          child: RaisedButton(
            padding: EdgeInsets.all(10),
            color: Colors.redAccent,
            child: Row(
                children: <Widget>[
                  Icon(Icons.network_check),
                  const Text('Disconnect')
                ]),
            onPressed: state == MQTTCovidAppConnectionState.connected
                ? _disconnect
                : null, //
          ),
        ),
      ],
    );
  }

  // check the connection
  String checkConnection(MQTTCovidAppConnectionState state) {
    switch (state) {
      case MQTTCovidAppConnectionState.connected:
        return 'Connection successfull';
      case MQTTCovidAppConnectionState.connecting:
        return 'Connecting to uni-freiburg broker';
      case MQTTCovidAppConnectionState.disconnected:
        return 'Disconnected ';
    }
  }

  // init the CovidAppManager with host address,topic
  void CovidAppInit() {
   
    String osPrefix = 'Flutter_iOS';
    if (Platform.isAndroid) {
      osPrefix = 'Flutter_Android';
    }
    manager = CovidAppManager(
     // uni broker address
        host: 'earth.informatik.uni-freiburg.de',
        topic: 'ubiblab/sensor/occupancy',
        identifier: osPrefix,
        state: currentAppState);
    manager.initializeMQTTClient();
    manager.connect();


  }

  void _disconnect() {
    manager.disconnect();
    FlutterRingtonePlayer.stop();
  }
}


