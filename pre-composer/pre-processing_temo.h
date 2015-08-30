void prepare_match_field(uint8_t * pkt , uint8_t * match_field){

    int length = pkt[0];

    int i=0;
    int j=0;
    int previous_length=1;
    int current_length=0;


    // Adding ingress port
    current_length = ingress_port/8;
    for(i=previous_length;i<=current_length;i++){
        match_field[i-1] = pkt[i]
    }
    previous_length=current_length+1;


    // Adding metadata
    current_length=current_length+metadata/8;
    for(i=previous_length;i<=current_length;i++){
        match_field[i-1] = pkt[i]
    }
    previous_length=current_length+1;

    // Adding destination mac
    current_length=current_length+mac_dst/8;
    for(i=previous_length;i<=current_length;i++){
        match_field[(mac_dst/8)+i-1] = pkt[i]
    }
    previous_length=current_length+1;

    // Adding source mac
    current_length=current_length+mac_src/8;
    for(i=previous_length;i<=current_length;i++){
        match_field[i-1-(mac_src/8)] = pkt[i]
    }
    previous_length=current_length+1;

    // Adding ethr type
    current_length=current_length+eth_typ/8;
    for(i=previous_length;i<=current_length;i++){
        match_field[i-1] = pkt[i]
    }
    previous_length=current_length+1;



    if(pkt[25]==8 && pkt[26]==0){    // Ipv4

        int ipv4_header_length = pkt[29]*16 + pkt[30];

        // Adding vlan,MPLS as zeros
        for(i=27;i<=31;i++){
            match_field[i-1] = 0;
        }

        // Adding IPV4 source and destination
        for(j=0;j<8;j++){

                for(i=1;i<=2;i++){
                    if(read_bit(pkt[39+j],i)==1){
                        set_bit(match_field[31+j],i+6)
                    }else{
                        reset_bit(match_field[31+j],i+6)
                    }
                }

                for(i=3;i<=8;i++){
                    if(read_bit(pkt[39+j],i)==1){
                        set_bit(match_field[32+j],i-2)
                    }else{
                        reset_bit(match_field[32+j],i-2)
                    }
                }
        }

        //Adding IPv4 protocol
        for(i=1;i<=2;i++){
            if(read_bit(pkt[36],i)==1){
                set_bit(match_field[38],i+6)
            }else{
                reset_bit(match_field[38],i+6)
            }
        }

        for(i=3;i<=8;i++){
            if(read_bit(pkt[36],i)==1){
                set_bit(match_field[39],i-2)
            }else{
                reset_bit(match_field[39],i-2)
            }
        }

        // Adding IPv4 Tos
        for(i=1;i<=2;i++){
            if(read_bit(pkt[28],i)==1){
                set_bit(match_field[39],i+6)
            }else{
                reset_bit(match_field[39],i+6)
            }
        }
        for(i=3;i<=8;i++){
            if(read_bit(pkt[28],i)==1){
                set_bit(match_field[40],i-2)
            }else{
                reset_bit(match_field[40],i-2)
            }
        }


        // Adding TCP source and destination
        for(j=0;j<4;j++){

                for(i=1;i<=2;i++){
                    if(read_bit(pkt[47+j],i)==1){
                        set_bit(match_field[40+j],i+6)
                    }else{
                        reset_bit(match_field[40+j],i+6)
                    }
                }

                for(i=3;i<=8;i++){
                    if(read_bit(pkt[47+j],i)==1){
                        set_bit(match_field[41+j],i-2)
                    }else{
                        reset_bit(match_field[41+j],i-2)
                    }
                }
        }










    }


}
