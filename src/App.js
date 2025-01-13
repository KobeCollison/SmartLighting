import logo from './logo.svg';
import './App.css';
import './EditColorAnimation.css';
import {StyleSelect,EditColorMenu} from './EditColorAnimation';
import ColorAnimation from './ColorAnimation';
import React, { useEffect, useState }  from 'react';
import {PostSender,NodePicker} from './web_com';

function App() {
  const [message, setMessage] = useState('');

  const ws = new WebSocket("wss://ws.kclight")


  // const colorPreview = new ColorAnimation();
  return (
    <div className="App"> 
      <header className="App-header">
        {/* <NodePicker></NodePicker> */}
        {/* <PostSender></PostSender> */}
        <p>
          {/* <div><StyleSelect /></div>  */}
          {/* Edit <code>src/App.js</code> and save to reload.<br/> */}
          <EditColorMenu />

          {/* {colorPreview.previewBlock()} */}
        </p>
      </header>
      
    </div>
  );
}

export default App;