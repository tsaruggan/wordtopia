import { useState } from 'react';
import Search from './Search';

function App() {

  const getSuggestions = async (prefix) => {
    const response = await fetch(`/suggest?prefix=${prefix}`);
    const data = await response.json();
    return data;
  }

  return (
    <>
      <div className='page'>
        <div className='main'>
          <div className='header'>
            <h1>Metaphoria</h1>
          </div>
          <Search getSuggestions={getSuggestions} />
          
        </div>
      </div>
    </>
  );
}

export default App;
