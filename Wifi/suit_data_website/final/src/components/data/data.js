import React, { Component } from 'react';
import axios from 'axios';

class Data extends Component {
  constructor(data) {
    super();
    // console.log(data.interval);
    this.state = {
      data: {},
      dataStr: '',
      interval: data.interval,
    };
  }

  getData = () => {
    // url for testing: https://dog.ceo/api/breeds/list/all
    // response.data.message;
    axios.get('http://192.168.1.1/getdata').then(response => {
      let message = response.data;
      let messageStr = JSON.stringify(message);
      console.log(messageStr);
      this.setState({
        data: message,
        dataStr: messageStr,
      });
    });
  };

  componentDidMount() {
    this.getData();
    this.requestInterval = setInterval(this.getData, this.state.interval);
  }

  componentWillUnmount() {
    clearInterval(this.requestInterval);
  }

  render() {
    if (this.state.dataStr.length === 0) return <div />;
    return (
      <div>
        <p>{this.state.dataStr}</p>
      </div>
    );
  }
}

export default Data;
